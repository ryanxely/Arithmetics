// =============================================================================
//  Arithmetics — Modern ImGui Application
//  src/ui/arithmetics.cpp
//
//  Architecture:
//    - AppState        : global UI state (current page, inputs, results)
//    - RenderSidebar() : left navigation panel
//    - RenderHome()    : dashboard with module cards
//    - RenderNumericMethods() : Newton-Raphson solver panel
//    - ApplyTheme()     : full custom ImGui style
//    - main()          : SDL2 + OpenGL3 init, main loop, cleanup
//
//  Adding a new module later:
//    1. Add a Page enum value
//    2. Add a card in RenderHome()
//    3. Add a RenderModule() function
//    4. Add a case in the main render switch
// =============================================================================

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <stdio.h>
#include <string.h>
#include <math.h>

// ── Link to C core ────────────────────────────────────────────────────────────
extern "C" {
    #include "core/numeric_methods.h"
}

// =============================================================================
//  DESIGN TOKENS — change these to retheme the entire app
// =============================================================================
namespace Theme {
    // Background layers
    static ImVec4 BgDeep      = {0.07f, 0.08f, 0.10f, 1.00f};  // window bg
    static ImVec4 BgPanel     = {0.11f, 0.12f, 0.15f, 1.00f};  // panels
    static ImVec4 BgCard      = {0.14f, 0.16f, 0.20f, 1.00f};  // cards
    static ImVec4 BgCardHover = {0.18f, 0.20f, 0.26f, 1.00f};  // card hover
    static ImVec4 BgInput     = {0.10f, 0.11f, 0.14f, 1.00f};  // input fields

    // Accent — teal/cyan
    static ImVec4 Accent      = {0.18f, 0.75f, 0.65f, 1.00f};
    static ImVec4 AccentDim   = {0.12f, 0.50f, 0.43f, 1.00f};
    static ImVec4 AccentHover = {0.24f, 0.88f, 0.76f, 1.00f};

    // Text
    static ImVec4 TextPrimary = {0.92f, 0.93f, 0.95f, 1.00f};
    static ImVec4 TextMuted   = {0.50f, 0.53f, 0.60f, 1.00f};
    static ImVec4 TextAccent  = {0.18f, 0.75f, 0.65f, 1.00f};

    // Status
    static ImVec4 Success     = {0.25f, 0.80f, 0.45f, 1.00f};
    static ImVec4 Error       = {0.90f, 0.35f, 0.35f, 1.00f};
    static ImVec4 Warning     = {0.95f, 0.70f, 0.20f, 1.00f};

    // Sidebar
    static ImVec4 SidebarBg   = {0.09f, 0.10f, 0.13f, 1.00f};
    static ImVec4 SidebarSel  = {0.14f, 0.16f, 0.21f, 1.00f};

    // Separator
    static ImVec4 Separator   = {0.20f, 0.22f, 0.28f, 1.00f};
}

// =============================================================================
//  APP STATE
// =============================================================================
enum class Page {
    Home,
    NumericMethods,
    // Future modules added here:
    // LinearAlgebra,
    // Statistics,
    // ComplexNumbers,
};

struct AppState {
    Page current_page = Page::Home;

    // ── Newton-Raphson inputs ──
    char    expr[256]    = "x^2 - 2";
    float   x0           = 1.0f;
    float   tol          = 1e-6f;
    int     max_iter     = 100;
    int     method_idx   = 0;   // 0 = Newton-Raphson (only one for now)

    // ── Result ──
    NRResult result      = {};
    bool     has_result  = false;
    bool     show_table  = true;
};

static AppState g_state;

// =============================================================================
//  THEME APPLICATION
// =============================================================================
static void ApplyTheme() {
    ImGuiStyle& s = ImGui::GetStyle();

    // Rounding
    s.WindowRounding    = 10.0f;
    s.ChildRounding     = 8.0f;
    s.FrameRounding     = 6.0f;
    s.PopupRounding     = 6.0f;
    s.ScrollbarRounding = 6.0f;
    s.GrabRounding      = 4.0f;
    s.TabRounding       = 6.0f;

    // Spacing
    s.WindowPadding     = {16.0f, 16.0f};
    s.FramePadding      = {10.0f,  7.0f};
    s.ItemSpacing       = {10.0f,  8.0f};
    s.ItemInnerSpacing  = { 8.0f,  6.0f};
    s.ScrollbarSize     = 10.0f;
    s.GrabMinSize       = 10.0f;
    s.WindowBorderSize  = 0.0f;
    s.FrameBorderSize   = 0.0f;
    s.ChildBorderSize   = 0.0f;

    // Colors
    ImVec4* c = s.Colors;
    c[ImGuiCol_WindowBg]          = Theme::BgDeep;
    c[ImGuiCol_ChildBg]           = Theme::BgPanel;
    c[ImGuiCol_PopupBg]           = Theme::BgPanel;

    c[ImGuiCol_FrameBg]           = Theme::BgInput;
    c[ImGuiCol_FrameBgHovered]    = Theme::BgCard;
    c[ImGuiCol_FrameBgActive]     = Theme::BgCard;

    c[ImGuiCol_TitleBg]           = Theme::BgDeep;
    c[ImGuiCol_TitleBgActive]     = Theme::BgDeep;
    c[ImGuiCol_TitleBgCollapsed]  = Theme::BgDeep;

    c[ImGuiCol_ScrollbarBg]       = Theme::BgDeep;
    c[ImGuiCol_ScrollbarGrab]     = Theme::AccentDim;
    c[ImGuiCol_ScrollbarGrabHovered] = Theme::Accent;
    c[ImGuiCol_ScrollbarGrabActive]  = Theme::AccentHover;

    c[ImGuiCol_CheckMark]         = Theme::Accent;
    c[ImGuiCol_SliderGrab]        = Theme::Accent;
    c[ImGuiCol_SliderGrabActive]  = Theme::AccentHover;

    c[ImGuiCol_Button]            = Theme::AccentDim;
    c[ImGuiCol_ButtonHovered]     = Theme::Accent;
    c[ImGuiCol_ButtonActive]      = Theme::AccentHover;

    c[ImGuiCol_Header]            = Theme::BgCard;
    c[ImGuiCol_HeaderHovered]     = Theme::BgCardHover;
    c[ImGuiCol_HeaderActive]      = Theme::AccentDim;

    c[ImGuiCol_Separator]         = Theme::Separator;
    c[ImGuiCol_SeparatorHovered]  = Theme::Accent;
    c[ImGuiCol_SeparatorActive]   = Theme::AccentHover;

    c[ImGuiCol_Tab]               = Theme::BgPanel;
    c[ImGuiCol_TabHovered]        = Theme::Accent;
    c[ImGuiCol_TabActive]         = Theme::AccentDim;
    c[ImGuiCol_TabUnfocused]      = Theme::BgPanel;
    c[ImGuiCol_TabUnfocusedActive]= Theme::BgCard;

    c[ImGuiCol_TableHeaderBg]     = Theme::BgCard;
    c[ImGuiCol_TableBorderLight]  = Theme::Separator;
    c[ImGuiCol_TableBorderStrong] = Theme::Separator;
    c[ImGuiCol_TableRowBg]        = {0.00f, 0.00f, 0.00f, 0.00f};
    c[ImGuiCol_TableRowBgAlt]     = {1.00f, 1.00f, 1.00f, 0.03f};

    c[ImGuiCol_Text]              = Theme::TextPrimary;
    c[ImGuiCol_TextDisabled]      = Theme::TextMuted;

    c[ImGuiCol_NavHighlight]      = Theme::Accent;
    c[ImGuiCol_ResizeGrip]        = Theme::AccentDim;
    c[ImGuiCol_ResizeGripHovered] = Theme::Accent;
    c[ImGuiCol_ResizeGripActive]  = Theme::AccentHover;
}

// =============================================================================
//  HELPER WIDGETS
// =============================================================================

// Labelled input row — label on the left, widget on the right
static void LabeledRow(const char* label) {
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
    ImGui::TextColored(Theme::TextMuted, "%s", label);
    ImGui::TableNextColumn();
}

// Pill-style badge
static void Badge(const char* text, ImVec4 color) {
    ImVec2 p    = ImGui::GetCursorScreenPos();
    ImVec2 size = ImGui::CalcTextSize(text);
    float  pad  = 6.0f;
    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(
        {p.x - pad,          p.y - 2.0f},
        {p.x + size.x + pad, p.y + size.y + 2.0f},
        ImGui::ColorConvertFloat4ToU32({color.x, color.y, color.z, 0.20f}),
        4.0f
    );
    ImGui::TextColored(color, "%s", text);
}

// Styled card background — call before content, EndCard() after
static bool BeginCard(const char* id, ImVec2 size = {0, 0}) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::BgCard);
    bool ok = ImGui::BeginChild(id, size, false,
                ImGuiWindowFlags_NoScrollbar |
                ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::SetCursorPos({ImGui::GetCursorPos().x + 12, ImGui::GetCursorPos().y + 12});
    return ok;
}

static void EndCard() {
    ImGui::EndChild();
    ImGui::PopStyleColor();
}

// Full-width accent button
static bool AccentButton(const char* label, ImVec2 size = {0, 36}) {
    ImGui::PushStyleColor(ImGuiCol_Button,        Theme::AccentDim);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Theme::Accent);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  Theme::AccentHover);
    bool clicked = ImGui::Button(label, size);
    ImGui::PopStyleColor(3);
    return clicked;
}

// =============================================================================
//  SIDEBAR
// =============================================================================
static void RenderSidebar(float sidebar_w, float win_h) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::SidebarBg);
    ImGui::BeginChild("##sidebar", {sidebar_w, win_h}, false);

    // ── App title ────────────────────────────────
    ImGui::SetCursorPos({16, 20});
    ImGui::TextColored(Theme::Accent, "ARITHMETICS");
    ImGui::SetCursorPosX(16);
    ImGui::TextColored(Theme::TextMuted, "v1.0 — Numerical Tools");
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 16);
    ImGui::Separator();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8);

    // ── Navigation items ─────────────────────────
    struct NavItem { const char* icon; const char* label; Page page; };
    static NavItem nav[] = {
        { "##",  "  Dashboard",       Page::Home          },
        { "##",  "  Numeric Methods", Page::NumericMethods },
        // Add new modules here as you build them
    };

    for (auto& item : nav) {
        bool selected = (g_state.current_page == item.page);
        if (selected)
            ImGui::PushStyleColor(ImGuiCol_Header, Theme::SidebarSel);
        else
            ImGui::PushStyleColor(ImGuiCol_Header, {0,0,0,0});

        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, Theme::SidebarSel);

        ImGui::SetCursorPosX(8);
        if (selected)
            ImGui::PushStyleColor(ImGuiCol_Text, Theme::Accent);

        bool clicked = ImGui::Selectable(item.label, selected,
                            ImGuiSelectableFlags_None, {sidebar_w - 16, 36});
        if (selected)
            ImGui::PopStyleColor();  // text color

        ImGui::PopStyleColor(2);    // header colors

        if (clicked)
            g_state.current_page = item.page;
    }

    // ── Bottom info ───────────────────────────────
    float bottom_y = win_h - 60.0f;
    ImGui::SetCursorPosY(bottom_y);
    ImGui::Separator();
    ImGui::SetCursorPosY(bottom_y + 8);
    ImGui::SetCursorPosX(16);
    ImGui::TextColored(Theme::TextMuted, "Faculté des Sciences");
    ImGui::SetCursorPosX(16);
    ImGui::TextColored(Theme::TextMuted, "TPE INF448");

    ImGui::EndChild();
    ImGui::PopStyleColor();
}

// =============================================================================
//  HOME — Dashboard
// =============================================================================
static void RenderHome(float content_w, float content_h) {
    ImGui::SetCursorPos({0, 0});

    // ── Page header ──────────────────────────────
    ImGui::TextColored(Theme::TextPrimary, "Tableau de Bord");
    ImGui::TextColored(Theme::TextMuted,
        "Choose a module to begin computing.");
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 16);
    ImGui::Separator();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);

    // ── Section label ────────────────────────────
    ImGui::TextColored(Theme::TextMuted, "AVAILABLE MODULES");
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

    // ── Module cards ─────────────────────────────
    // Card layout: two per row, fixed size
    float card_w = (content_w - 20.0f) * 0.5f;
    float card_h = 130.0f;

    // Card 1 — Numeric Methods (available)
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::BgCard);
        ImGui::BeginChild("##card_nm", {card_w, card_h}, false);

        ImGui::SetCursorPos({14, 14});
        ImGui::TextColored(Theme::Accent, "Numeric Methods");
        ImGui::SetCursorPos({14, 38});
        ImGui::TextColored(Theme::TextMuted,
            "Root finding algorithms.");
        ImGui::SetCursorPos({14, 58});
        ImGui::TextColored(Theme::TextMuted,
            "Newton-Raphson iteration");
        ImGui::SetCursorPos({14, 82});
        Badge("Available", Theme::Success);

        ImGui::SetCursorPos({14, card_h - 34});
        if (AccentButton("Open##nm", {card_w - 28, 28}))
            g_state.current_page = Page::NumericMethods;

        ImGui::EndChild();
        ImGui::PopStyleColor();
    }

    ImGui::SameLine(0, 20);

    // Card 2 — Placeholder for future module
    // {
    //     ImGui::PushStyleColor(ImGuiCol_ChildBg,
    //         {Theme::BgCard.x * 0.7f,
    //          Theme::BgCard.y * 0.7f,
    //          Theme::BgCard.z * 0.7f, 0.5f});
    //     ImGui::BeginChild("##card_future", {card_w, card_h}, false);

    //     ImGui::SetCursorPos({14, 14});
    //     ImGui::TextColored(Theme::TextMuted, "Linear Algebra");
    //     ImGui::SetCursorPos({14, 38});
    //     ImGui::TextColored(Theme::TextMuted,
    //         "Matrix operations,");
    //     ImGui::SetCursorPos({14, 58});
    //     ImGui::TextColored(Theme::TextMuted,
    //         "Gaussian elimination.");
    //     ImGui::SetCursorPos({14, 82});
    //     Badge("Coming soon", Theme::Warning);

    //     ImGui::EndChild();
    //     ImGui::PopStyleColor();
    // }

    // ── Stats row ────────────────────────────────
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 24);
    ImGui::TextColored(Theme::TextMuted, "SESSION");
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

    float stat_w = (content_w - 40.0f) / 3.0f;

    auto StatCard = [&](const char* id, const char* val, const char* lbl) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::BgCard);
        ImGui::BeginChild(id, {stat_w, 64}, false);
        ImGui::SetCursorPos({12, 10});
        ImGui::TextColored(Theme::Accent, "%s", val);
        ImGui::SetCursorPos({12, 34});
        ImGui::TextColored(Theme::TextMuted, "%s", lbl);
        ImGui::EndChild();
        ImGui::PopStyleColor();
    };

    // Count solved so far this session
    static int s_solves = 0;
    char solves_str[16];
    snprintf(solves_str, sizeof(solves_str), "%d", s_solves);

    StatCard("##s1", solves_str,  "Computations run");
    ImGui::SameLine(0, 20);
    StatCard("##s2",
        g_state.has_result && g_state.result.converged ? "Yes" : "—",
        "Last result converged");
    ImGui::SameLine(0, 20);
    StatCard("##s3",
        g_state.has_result ?
            (g_state.result.converged ? "Newton-Raphson" : "—") : "—",
        "Last method used");

    // Track solves (increment when we get a result from any module)
    static bool s_counted = false;
    if (g_state.has_result && !s_counted) {
        s_solves++;
        s_counted = true;
    }
    if (!g_state.has_result) s_counted = false;
}

// =============================================================================
//  NUMERIC METHODS PAGE
// =============================================================================
static void RenderNumericMethods(float content_w) {

    // ── Page header ──────────────────────────────
    ImGui::TextColored(Theme::TextPrimary, "Numeric Methods");
    ImGui::TextColored(Theme::TextMuted,
        "Iterative root-finding for real-valued functions.");
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 12);
    ImGui::Separator();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 16);

    // ── Two-column layout: inputs left, result right ──
    float left_w  = content_w * 0.40f;
    float right_w = content_w * 0.56f;
    float col_h   = 460.0f;

    // ── LEFT — Input panel ────────────────────────
    ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::BgCard);
    ImGui::BeginChild("##inputs", {left_w, col_h}, false);
    {
        ImGui::SetCursorPos({14, 14});
        ImGui::TextColored(Theme::TextMuted, "PARAMETERS");
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8);
        ImGui::SetCursorPosX(14);
        ImGui::Separator();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 12);

        // Method selector
        ImGui::SetCursorPosX(14);
        ImGui::TextColored(Theme::TextMuted, "Method");
        ImGui::SetCursorPosX(14);
        const char* methods[] = { "Newton-Raphson" };
        ImGui::SetNextItemWidth(left_w - 28);
        ImGui::Combo("##method", &g_state.method_idx, methods,
                     IM_ARRAYSIZE(methods));
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

        // Expression input
        ImGui::SetCursorPosX(14);
        ImGui::TextColored(Theme::TextMuted, "f(x)  — expression in x");
        ImGui::SetCursorPosX(14);
        ImGui::SetNextItemWidth(left_w - 28);
        ImGui::InputText("##expr", g_state.expr, sizeof(g_state.expr));
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

        // Numeric inputs in a table for alignment
        if (ImGui::BeginTable("##params", 2,
                ImGuiTableFlags_None, {left_w - 28, 0})) {
            ImGui::TableSetupColumn("label", ImGuiTableColumnFlags_WidthFixed,
                                    120.0f);
            ImGui::TableSetupColumn("field", ImGuiTableColumnFlags_WidthStretch);

            LabeledRow("x\u2080  (initial guess)");
            ImGui::SetNextItemWidth(-1);
            ImGui::InputFloat("##x0", &g_state.x0, 0.1f, 1.0f, "%.4f");

            LabeledRow("Tolerance");
            ImGui::SetNextItemWidth(-1);
            ImGui::InputFloat("##tol", &g_state.tol, 0, 0, "%.2e");

            LabeledRow("Max iterations");
            ImGui::SetNextItemWidth(-1);
            ImGui::InputInt("##maxiter", &g_state.max_iter);
            if (g_state.max_iter < 1)   g_state.max_iter = 1;
            if (g_state.max_iter > 500) g_state.max_iter = 500;

            ImGui::EndTable();
        }

        // Hints
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
        ImGui::SetCursorPosX(14);
        ImGui::TextColored(Theme::TextMuted,
            "Supported: sin cos tan log ln");
        ImGui::SetCursorPosX(14);
        ImGui::TextColored(Theme::TextMuted,
            "Operators: + - * / ^ ( )");
        ImGui::SetCursorPosX(14);
        ImGui::TextColored(Theme::TextMuted,
            "Variable:  x");

        // Solve button — pinned to bottom of card
        ImGui::SetCursorPosY(col_h - 52);
        ImGui::SetCursorPosX(14);
        if (AccentButton("  Solve  ", {left_w - 28, 36})) {
            if (g_state.has_result)
                nr_free(&g_state.result);
            g_state.result     = newton_raphson(
                g_state.expr, g_state.x0,
                g_state.tol,  g_state.max_iter);
            g_state.has_result = true;
        }
    }
    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::SameLine(0, 16);

    // ── RIGHT — Results panel ─────────────────────
    ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::BgCard);
    ImGui::BeginChild("##results", {right_w, col_h}, false);
    {
        ImGui::SetCursorPos({14, 14});
        ImGui::TextColored(Theme::TextMuted, "RESULT");
        ImGui::SetCursorPosX(14);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8);
        ImGui::Separator();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 12);

        if (!g_state.has_result) {
            // Empty state
            ImGui::SetCursorPosY(col_h * 0.35f);
            float tw = ImGui::CalcTextSize("Enter parameters and press Solve.").x;
            ImGui::SetCursorPosX((right_w - tw) * 0.5f);
            ImGui::TextColored(Theme::TextMuted,
                "Enter parameters and press Solve.");
        } else {
            NRResult* r = &g_state.result;

            // ── Summary badges ──
            ImGui::SetCursorPosX(14);
            if (r->converged) {
                Badge("Converged", Theme::Success);
                ImGui::SameLine();
                char root_str[64];
                snprintf(root_str, sizeof(root_str),
                         "  Root \u2248 %.8f", r->root);
                ImGui::TextColored(Theme::TextPrimary, "%s", root_str);

                ImGui::SetCursorPosX(14);
                char iter_str[64];
                snprintf(iter_str, sizeof(iter_str),
                         "%d iterations", r->iter_count);
                ImGui::TextColored(Theme::TextMuted, "%s", iter_str);
            } else {
                Badge("Did not converge", Theme::Error);
                ImGui::SetCursorPosX(14);
                ImGui::TextColored(Theme::TextMuted,
                    "Try a different x\u2080 or increase max iterations.");
            }

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 12);

            // ── Toggle table ──
            ImGui::SetCursorPosX(14);
            ImGui::Checkbox("Show iteration table", &g_state.show_table);

            if (g_state.show_table) {
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8);

                float tbl_h = col_h - ImGui::GetCursorPosY() - 14.0f;

                if (ImGui::BeginTable("##itertable", 6,
                        ImGuiTableFlags_Borders        |
                        ImGuiTableFlags_RowBg          |
                        ImGuiTableFlags_ScrollY        |
                        ImGuiTableFlags_SizingFixedFit |
                        ImGuiTableFlags_NoHostExtendX,
                        {right_w - 14, tbl_h})) {

                    // Column headers
                    ImGui::TableSetupScrollFreeze(0, 1);
                    ImGui::TableSetupColumn("n",        ImGuiTableColumnFlags_WidthFixed, 30);
                    ImGui::TableSetupColumn("x_n",      ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("f(x_n)",   ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("f'(x_n)",  ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("x_(n+1)",  ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("delta_x",  ImGuiTableColumnFlags_WidthFixed, 72);
                    ImGui::TableHeadersRow();

                    // Rows
                    for (int i = 0; i < r->iter_count; i++) {
                        NRIteration* row = &r->iterations[i];
                        ImGui::TableNextRow();

                        // Highlight the last (converged) row
                        if (i == r->iter_count - 1 && r->converged)
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,
                                ImGui::ColorConvertFloat4ToU32(
                                    {Theme::Accent.x, Theme::Accent.y,
                                     Theme::Accent.z, 0.12f}));

                        ImGui::TableNextColumn();
                        ImGui::TextColored(Theme::TextMuted, "%d", row->n);
                        ImGui::TableNextColumn();
                        ImGui::Text("%.6f", row->x_n);
                        ImGui::TableNextColumn();
                        ImGui::Text("%.6f", row->fx_n);
                        ImGui::TableNextColumn();
                        ImGui::Text("%.6f", row->dfx_n);
                        ImGui::TableNextColumn();
                        ImGui::Text("%.6f", row->x_next);
                        ImGui::TableNextColumn();
                        // Color delta_x: red when large, green when small
                        float d = row->delta_x;
                        ImVec4 dc = (d < g_state.tol * 10)
                                    ? Theme::Success : Theme::TextPrimary;
                        ImGui::TextColored(dc, "%.2e", d);
                    }
                    ImGui::EndTable();
                }
            }
        }
    }
    ImGui::EndChild();
    ImGui::PopStyleColor();
}

// =============================================================================
//  MAIN
// =============================================================================
int main(int argc, char* argv[]) {

    // ── SDL2 init ─────────────────────────────────
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_Window* window = SDL_CreateWindow(
        "Projet TPE — Arithmetics",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1200, 720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
        SDL_WINDOW_ALLOW_HIGHDPI
    );
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GLContext gl_ctx = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_ctx);
    SDL_GL_SetSwapInterval(1);

    // ── ImGui init ────────────────────────────────
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename  = nullptr;   // no imgui.ini file saved

    ApplyTheme();

    ImGui_ImplSDL2_InitForOpenGL(window, gl_ctx);
    ImGui_ImplOpenGL3_Init("#version 150");

    // ── Main loop ─────────────────────────────────
    bool running = true;
    while (running) {

        // Events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(window))
                running = false;
        }

        // New frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // ── Full-screen host window ───────────────
        ImGui::SetNextWindowPos({0, 0});
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::Begin("##host",nullptr,
            ImGuiWindowFlags_NoTitleBar      |
            ImGuiWindowFlags_NoResize        |
            ImGuiWindowFlags_NoMove          |
            ImGuiWindowFlags_NoScrollbar     |
            ImGuiWindowFlags_NoScrollWithMouse|
            ImGuiWindowFlags_NoBringToFrontOnFocus);

        float win_w = io.DisplaySize.x;
        float win_h = io.DisplaySize.y;
        float sidebar_w  = 210.0f;
        float content_pad = 24.0f;
        float content_w  = win_w - sidebar_w - content_pad * 2;

        // ── Sidebar ───────────────────────────────
        ImGui::SetCursorPos({0, 0});
        RenderSidebar(sidebar_w, win_h);

        // ── Content area ──────────────────────────
        ImGui::SetCursorPos({sidebar_w + content_pad, content_pad});
        ImGui::BeginChild("##content",
            {content_w, win_h - content_pad * 2}, false);

        switch (g_state.current_page) {
            case Page::Home:
                RenderHome(content_w, win_h - content_pad * 2);
                break;
            case Page::NumericMethods:
                RenderNumericMethods(content_w);
                break;
        }

        ImGui::EndChild();
        ImGui::End();

        // ── Render ────────────────────────────────
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0.07f, 0.08f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // ── Cleanup ───────────────────────────────────
    if (g_state.has_result)
        nr_free(&g_state.result);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl_ctx);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}