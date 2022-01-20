#include <stdio.h>
//#include "glad/glad.h"

#include "glad.c"
#include <GLFW/glfw3.h>

#define SI_MEMORY_IMPLEMENTATION
#include "si_memory.h"

#include "types.h"

#include "si_math.h"
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <x86intrin.h>

#include "8080.c"
#include "decompiler_debug.c"
#include "read_file.c"

//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

const char *vert_shader_text
    = "#version 430\n"
      "layout (location = 0) in vec3 vPos;\n"
      "layout (location = 1) in vec2 UV;\n"
      "out vec2 uv;\n"
      "uniform mat4 MVP;\n"
      "void main()\n"
      "{\n"
      "    gl_Position = MVP * vec4(vPos, 1.0);\n"
      "    uv = UV;\n"
      //"    velocity = vVelocity;\n"
      "}\n";

const char *frag_shader_text
    = "#version 430\n"
      "out vec4 FragColor;\n"
      "in vec2 uv;"
      "uniform vec4 color;"
      "void main()\n"
      "{\n"
      "    FragColor = vec4(color);\n"
      "}\n";

static inline uint64_t
get_cycle_count()
{
    return __rdtsc();
}

#define START_CYCLE_COUNT(name) uint64_t name##startCount = get_cycle_count();
#define END_CYCLE_COUNT(name)                                \
    uint64_t name##endCount = get_cycle_count();             \
    uint64_t name##diff = name##endCount - name##startCount; \
    printf("%s : %" PRIu64 "cycles, %" PRIu64 "k cycles, %" PRIu64 "m cycles\n", #name, name##diff, (name##diff) / 1000, (name##diff) / 1000000);

struct game_memory {
    si_primary_buffer primaryBuffer;
    si_memory_arena mainArena;
};

typedef struct {
    si_v3 position;
    si_v2 uv;
} vertex_data;

void APIENTRY opengl_debug_callback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar *message,
    const void *userParam)
{
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH: {
            fprintf(stderr, "Severe Opengl Message: %s\n", message);
            assert(false);
        } break;
        case GL_DEBUG_SEVERITY_MEDIUM: {
            fprintf(stderr, "Medium severity Opengl Message: %s\n", message);
            assert(false);
        } break;
        case GL_DEBUG_SEVERITY_LOW: {
            // RendererLog("Low severity Opengl message: %s\r\n", message);
        } break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: {
            // RendererLog("Low severity Opengl message: %s\r\n", message);
        } break;
    }
}

internal const char *
get_gl_error_string(GLenum error)
{
    switch (error) {
        case GL_INVALID_ENUM:
            return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:
            return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION";
        case GL_STACK_OVERFLOW:
            return "GL_STACK_OVERFLOW";
        case GL_STACK_UNDERFLOW:
            return "GL_STACK_UNDERFLOW: Given when a stack popping operation cannot be done because the stack is already at its lowest point";
        case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY:";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case GL_CONTEXT_LOST:
            return "GL_CONTEXT_LOST";
        default: {
            assert(!"Invalid opengl error code");
            return "Invalid opengl error code";
        } break;
    }
}

#define array_count(a) (sizeof(a) / sizeof(a[0]))

internal int
report_errors()
{
    const char *strings[5] = {};
    i32 maxStrings = array_count(strings);
    i32 numStrings = 0;
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR && numStrings < maxStrings) {
        strings[numStrings++] = get_gl_error_string(error);
    }

    for (i32 i = 0; i < numStrings; ++i) {
        fprintf(stderr, "\n\nOpengl error: %s\n\n", strings[i]);
    }

    return numStrings > 0;
}

internal GLuint
create_shader(GLenum type, const char *code, si_memory_arena *arena)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &code, NULL);
    glCompileShader(shader);

    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        int infoSize;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoSize);

        si_temp_memory tempMem = si_start_temp_memory(arena);
        {
            char *log = si_push_array(arena, infoSize, char);
            glGetShaderInfoLog(shader, infoSize, NULL, log);
            fprintf(stderr, "%s\n%s\n", code, log);
        }
        si_pop_temp_memory(tempMem);
        assert(0);
    }

    assert(!report_errors());
    return shader;
}

internal GLuint
create_program(GLuint vShader, GLuint fShader, si_memory_arena *arena)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, vShader);
    glAttachShader(program, fShader);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        int infoSize;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoSize);

        si_temp_memory tempMem = si_start_temp_memory(arena);
        {
            char *log = si_push_array(arena, infoSize, char);
            glGetProgramInfoLog(program, infoSize, NULL, log);
            fprintf(stderr, "%s\n", log);
        }
        si_pop_temp_memory(tempMem);
        assert(0);
    }

    assert(!report_errors());
    return program;
}

internal GLuint
create_program_from_files(const char *vertexShaderPath, const char *fragmentShaderPath, si_memory_arena *arena)
{
    struct read_file_result vCode = read_entire_file(vertexShaderPath, arena);
    struct read_file_result fCode = read_entire_file(fragmentShaderPath, arena);
    GLuint vShader = create_shader(GL_VERTEX_SHADER, (const char *)vCode.contents, arena);
    GLuint fShader = create_shader(GL_FRAGMENT_SHADER, (const char *)fCode.contents, arena);
    GLuint program = create_program(vShader, fShader, arena);
    glDeleteShader(vShader);
    glDeleteShader(fShader);
    assert(!report_errors());
    return program;
}

internal GLuint
create_program_from_strings(const char *vCode, const char *fCode, si_memory_arena *arena)
{
    GLuint vShader = create_shader(GL_VERTEX_SHADER, vCode, arena);
    GLuint fShader = create_shader(GL_FRAGMENT_SHADER, fCode, arena);
    GLuint program = create_program(vShader, fShader, arena);
    glDeleteShader(vShader);
    glDeleteShader(fShader);
    assert(!report_errors());
    return program;
}

internal GLuint
create_texture(const u32 *data, i32 w, i32 h, b32 useLinearColor)
{
    GLuint result;
    glGenTextures(1, &result);
    glBindTexture(GL_TEXTURE_2D, result);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (useLinearColor) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }

    glGenerateMipmap(GL_TEXTURE_2D);

    return result;
}

#if 0
internal GLuint
create_texture_from_file(const char *filename, b32 useLinearColor)
{
    i32 w, h, c;
    u32 *image = (u32 *)stbi_load(filename, &w, &h, &c, 4);
    assert(image);
    flip_pixels_vertically(image, w, h);
    GLuint result = create_texture(image, w, h, useLinearColor);
    STBI_FREE(image);
    return result;
}
#endif

static void
error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
    assert(0);
}

internal b32
contains_handle(i32 *handles, i32 count, i32 handleToCheck)
{
    for (i32 i = 0; i < count; ++i) {
        if (handles[i] == handleToCheck) {
            return true;
        }
    }
    return false;
}

internal GLuint
create_basic_array_buffer(GLsizei sizeOfData, const void *data)
{
    GLuint result;
    glGenBuffers(1, &result);
    glBindBuffer(GL_ARRAY_BUFFER, result);
    glBufferData(GL_ARRAY_BUFFER, sizeOfData, data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)(sizeof(float) * 3));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return result;
}

internal void
print_state(struct cpu_8080 *s)
{
    printf("PC: %hu | SP: %hu | IE: %u\n", s->pc, s->sp, s->interruptEnabled);
    printf("A: %u | B: %u | C: %u | D: %u | E %u | H %u | L %u\n", s->a, s->b, s->c, s->d, s->e, s->h, s->l);
    printf("Z: %u | S: %u | P: %u | CY: %u | AC: %u\n\n", s->cc.z, s->cc.s, s->cc.p, s->cc.cy, s->cc.ac);
}

internal inline void
machine_out(struct machine_8080 *machine, u8 portNum, u8 value)
{
    switch (portNum) {
        case 2: {
            machine->shiftOffset = value & 0x7;
        } break;
        case 4: {
            machine->shift[0] = machine->shift[1];
            machine->shift[1] = value;
        } break;
    }
}

internal inline u8
machine_in(struct machine_8080 *machine, u8 portNum)
{
    u8 a = 0;
    switch (portNum) {
        case 3: {
            u16 v = (machine->shift[0] << 8) | machine->shift[1];
            a = ((v >> (8 - machine->shiftOffset)) & 0xff);
        } break;
    }
    return a;
}

internal inline void
machine_key_down(struct machine_8080 *machine, char key)
{
    switch (key) {
        case 0: machine->ports[1] |= 0x20; break; // Set bit 5 of port 1
        case 1: machine->ports[1] |= 0x40; break; // Set bit 6 of port 1
    }
}

internal inline void
machine_key_up(struct machine_8080 *machine, char key)
{
    switch (key) {
        case 0: machine->ports[1] &= 0xDF; break; // clear bit 5 of port 1
        case 1: machine->ports[1] &= 0xBF; break; // clear bit 6 of port 1
    }
}

internal void
key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    struct machine_8080 *machine = glfwGetWindowUserPointer(window);
    switch (key) {
        case GLFW_KEY_A: {
            if (action == GLFW_PRESS) {
                machine_key_down(machine, 0);
            } else if (action == GLFW_RELEASE) {
                machine_key_up(machine, 0);
            }
        } break;
        case GLFW_KEY_D: {
            if (action == GLFW_PRESS) {
                machine_key_down(machine, 1);
            } else if (action == GLFW_RELEASE) {
                machine_key_up(machine, 1);
            }
        } break;
        case GLFW_KEY_ESCAPE: {
            if (action == GLFW_RELEASE) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
        } break;
    }
}
int main(void)
{
    struct game_memory gameMemory = { 0 };
    gameMemory.primaryBuffer = si_allocate_primary_buffer(si_megabytes(256), 0);
    si_initialize_arena(&gameMemory.mainArena, si_megabytes(128), gameMemory.primaryBuffer.data);

    struct game_memory *gm = &gameMemory;

    struct read_file_result program = read_entire_file("invaders/invaders", &gm->mainArena);
    // struct read_file_result program = read_entire_file("cpudiag.bin", &gm.mainArena);
    assert(program.contents);

    struct cpu_8080 cpu = {};
    u16 stackSize = si_kilobytes(4);
    cpu.m = si_push_size(&gm->mainArena, program.contentsSize + stackSize);
    memcpy(cpu.m, program.contents, program.contentsSize);

    struct machine_8080 machine = {};

    if (!glfwInit()) {
        printf("Failed to initialize glfw\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(1280, 1024, "Ass", NULL, NULL);
    if (!window) {
        printf("Failed to create window\n");
        glfwTerminate();
        return -1;
    }

    glfwSetErrorCallback(error_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowUserPointer(window, &machine);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(opengl_debug_callback, NULL);

    vertex_data quad[] = {
        { { 0.5f, 0.5f, 0.0f }, { 1.0f, 1.0f } },   // top right
        { { 0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f } },  // bottom right
        { { -0.5f, 0.5f, 0.0f }, { 0.0f, 1.0f } },  // top left
        { { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f } }, // bottom left
    };

    vertex_data selectionQuad[array_count(quad)];
    memcpy(selectionQuad, quad, sizeof(quad));

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // GLuint vbo = create_basic_array_buffer(sizeof(quad), quad);
    GLuint vbo = 0;
    i32 imageVAOVertCount = array_count(quad);
    GLuint screenshader = create_program_from_strings(vert_shader_text, frag_shader_text, &gameMemory.mainArena);

    glUseProgram(screenshader);

    GLint mvpUni = glGetUniformLocation(screenshader, "MVP");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    i32 winW, winH;
    glfwGetFramebufferSize(window, &winW, &winH);
    r32 ratio = winW / (float)winH;

#if 0
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }
#endif

    u64 instructionsExecuted = 0;
    print_state(&cpu);
    printf("Program size %td\n", program.contentsSize);

    while (!glfwWindowShouldClose(window)) {

        // print_8080_hex_instruction(&state.m[state.pc]);
        //  printf("\n");

        u8 *opcode = &cpu.m[cpu.pc];
        if (opcode[0] == 0xdb) { // IN
            u8 port = opcode[1];
            cpu.a = machine_in(&machine, port);
            cpu.pc += 2;
            // printf("MACHINE IN\n");
        } else if (opcode[0] == 0xd3) { // OUT
            u8 port = opcode[1];
            machine_out(&machine, port, 0);
            cpu.pc += 2;
            // printf("MACHINE OUT\n");
        } else {
            emulate_8080(&cpu);
        }

        instructionsExecuted++;
        // printf("Instuctions executed: %" PRIu64 "\n", instructionsExecuted);
        // print_state(&state);

        si_mat4x4 projection = si_orthographic(-ratio, ratio, -1.0f, 1.0f, -1.0f, 10.0f);

        si_v3 camPos = {};
        si_v3 camTarget = { 0.0f, 0.0f, 1.0f };
        si_v3 up = { 0.0f, 1.0f, 0.0f };
        si_mat4x4 view = si_mat4x4_identity();

        si_mat4x4 mvp = si_mat4x4_mul(projection, view);
        glClear(GL_COLOR_BUFFER_BIT);

        if (instructionsExecuted >= 50000) {
            glUniformMatrix4fv(mvpUni, 1, GL_FALSE, (r32 *)mvp.v);
            glViewport(0, 0, winW, winH);
            glClearColor(0, 0, 0, 0);

            // while ((getchar()) != '\n') {}
            // print_8080_hex_instruction(&state.m[state.pc]);
            // print_state(&state);

            glBindVertexArray(0);
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    glfwTerminate();
    si_free_primary_buffer(&gameMemory.primaryBuffer);
    return 0;
}
