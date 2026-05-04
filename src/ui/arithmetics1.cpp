#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <math.h>

// Your own core — untouched
extern "C" {
    #include "core/numeric_methods.h"
}

int main() {

    // ── 1. Init SDL2 ──────────────────────────────
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* window = SDL_CreateWindow(
        "Projet TPE - Arithmetics",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    SDL_GLContext gl_ctx = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_ctx);
    SDL_GL_SetSwapInterval(1);  // vsync

    // ── 2. Init ImGui ─────────────────────────────
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window, gl_ctx);
    ImGui_ImplOpenGL3_Init("#version 150");

    // ── 3. App state ──────────────────────────────
    char  expr[256]  = "x^2 - 2";
    float x0         = 1.0f;
    float tol        = 1e-6f;
    int   max_iter   = 100;

    NRResult result;
    strncpy(result.expr, expr, sizeof(result.expr) - 1);
    result.expr[sizeof(result.expr) - 1] = '\0';
    result.x0        = x0;
    result.tolerance = tol;
    result.max_iter  = max_iter;

    result.converged = 0;
    result.root      = NAN;
    result.iter_count = 0;

    bool  has_result = false;

    // ── 4. Main loop ──────────────────────────────
    bool running = true;
    while (running) {

        // Poll events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) running = false;
        }

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // ── 5. Your UI panels ─────────────────────

        ImGui::Begin("Newton-Raphson Solver");

            ImGui::InputText("f(x)", expr, sizeof(expr));
            ImGui::InputFloat("x0 (initial guess)", &x0);
            ImGui::InputFloat("Tolerance", &tol, 0, 0, "%.2e");
            ImGui::InputInt("Max iterations", &max_iter);

            if (ImGui::Button("Solve")) {
                if (has_result) nr_free(&result);
                result  = newton_raphson(expr, x0, tol, max_iter);
                has_result = true;
            }

            if (has_result) {
                ImGui::Separator();
                if (result.converged)
                    ImGui::Text("Root ~ %.7f  (%d iterations)",
                                result.root, result.iter_count);
                else
                    ImGui::TextColored({1,0.3f,0.3f,1}, "Did not converge.");

                // Iteration table
                if (ImGui::BeginTable("iterations", 6,
                    ImGuiTableFlags_Borders |
                    ImGuiTableFlags_RowBg   |
                    ImGuiTableFlags_ScrollY,
                    ImVec2(0, 300))) {

                    ImGui::TableSetupColumn("n");
                    ImGui::TableSetupColumn("x_n");
                    ImGui::TableSetupColumn("f(x_n)");
                    ImGui::TableSetupColumn("f'(x_n)");
                    ImGui::TableSetupColumn("x_(n+1)");
                    ImGui::TableSetupColumn("delta_x");
                    ImGui::TableHeadersRow();

                    for (int i = 0; i < result.iter_count; i++) {
                        NRIteration* row = &result.iterations[i];
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::Text("%d",    row->n);
                        ImGui::TableNextColumn(); ImGui::Text("%.6f",  row->x_n);
                        ImGui::TableNextColumn(); ImGui::Text("%.6f",  row->fx_n);
                        ImGui::TableNextColumn(); ImGui::Text("%.6f",  row->dfx_n);
                        ImGui::TableNextColumn(); ImGui::Text("%.6f",  row->x_next);
                        ImGui::TableNextColumn(); ImGui::Text("%.2e",  row->delta_x);
                    }
                    ImGui::EndTable();
                }
            }

        ImGui::End();

        // ── 6. Render ─────────────────────────────
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // ── 7. Cleanup ────────────────────────────────
    if (has_result) nr_free(&result);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl_ctx);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}