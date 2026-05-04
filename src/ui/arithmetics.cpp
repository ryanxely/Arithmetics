// =============================================================================
//  Arithmetics — Interface Graphique Moderne
//  src/ui/arithmetics.cpp
//
//  Modules :
//    - Page::Accueil              : tableau de bord + historique
//    - Page::MethodesNumeriques   : solveur Newton-Raphson
//    - Page::EvaluateurFonction   : évaluation f(x) + arbre AST
//    - Page::Parametres           : configuration (fichier historique...)
//
//  Pour ajouter un module :
//    1. Ajouter une valeur dans enum class Page
//    2. Ajouter une carte dans RenderAccueil()
//    3. Écrire une fonction RenderMonModule()
//    4. Ajouter un case dans le switch principal
// =============================================================================

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <string>

extern "C" {
    #include "core/numeric_methods.h"
    #include "core/math_parser.h"
}

// =============================================================================
//  TOKENS DE DESIGN
// =============================================================================
namespace Theme {
    static ImVec4 BgDeep       = {0.07f, 0.08f, 0.10f, 1.00f};
    static ImVec4 BgPanel      = {0.11f, 0.12f, 0.15f, 1.00f};
    static ImVec4 BgCard       = {0.13f, 0.15f, 0.19f, 1.00f};
    static ImVec4 BgInput      = {0.09f, 0.10f, 0.13f, 1.00f};
    static ImVec4 Accent       = {0.18f, 0.75f, 0.65f, 1.00f};
    static ImVec4 AccentDim    = {0.11f, 0.48f, 0.41f, 1.00f};
    static ImVec4 AccentHover  = {0.24f, 0.88f, 0.76f, 1.00f};
    static ImVec4 TextPrimary  = {0.92f, 0.93f, 0.95f, 1.00f};
    static ImVec4 TextMuted    = {0.48f, 0.52f, 0.58f, 1.00f};
    static ImVec4 Success      = {0.25f, 0.80f, 0.45f, 1.00f};
    static ImVec4 Error        = {0.90f, 0.35f, 0.35f, 1.00f};
    static ImVec4 Warning      = {0.95f, 0.70f, 0.20f, 1.00f};
    static ImVec4 SidebarBg    = {0.08f, 0.09f, 0.12f, 1.00f};
    static ImVec4 SidebarSel   = {0.13f, 0.15f, 0.20f, 1.00f};
    static ImVec4 Separator    = {0.18f, 0.20f, 0.26f, 1.00f};
    static float  FontSizeBase = 15.0f;
}

// =============================================================================
//  HISTORIQUE  (persistence CSV)
// =============================================================================
struct EntreeHistorique {
    char datetime[32];
    char module[64];
    char operation[256];
};

static std::vector<EntreeHistorique> g_historique;
static char g_history_path[512] = "history.csv";

static void HistoriqueCharger() {
    g_historique.clear();
    FILE* f = fopen(g_history_path, "r");
    if (!f) return;
    char line[512];
    while (fgets(line, sizeof(line), f)) {
        char* p1 = strchr(line, '|');
        char* p2 = p1 ? strchr(p1 + 1, '|') : nullptr;
        if (!p1 || !p2) continue;
        *p1 = '\0'; *p2 = '\0';
        char* nl = strchr(p2 + 1, '\n');
        if (nl) *nl = '\0';
        EntreeHistorique e = {};
        strncpy(e.datetime,  line,    sizeof(e.datetime)  - 1);
        strncpy(e.module,    p1 + 1,  sizeof(e.module)    - 1);
        strncpy(e.operation, p2 + 1,  sizeof(e.operation) - 1);
        g_historique.push_back(e);
    }
    fclose(f);
}

static void HistoriqueAjouter(const char* module, const char* operation) {
    EntreeHistorique e = {};
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    strftime(e.datetime, sizeof(e.datetime), "%Y-%m-%d %H:%M:%S", t);
    strncpy(e.module,    module,    sizeof(e.module)    - 1);
    strncpy(e.operation, operation, sizeof(e.operation) - 1);
    g_historique.push_back(e);
    FILE* f = fopen(g_history_path, "a");
    if (!f) return;
    fprintf(f, "%s|%s|%s\n", e.datetime, e.module, e.operation);
    fclose(f);
}

static void HistoriqueEffacer() {
    g_historique.clear();
    FILE* f = fopen(g_history_path, "w");
    if (f) fclose(f);
}

// =============================================================================
//  ETAT GLOBAL
// =============================================================================
enum class Page {
    Accueil,
    MethodesNumeriques,
    EvaluateurFonction,
    Parametres,
};

struct EtatApp {
    Page  page_courante     = Page::Accueil;

    // Newton-Raphson
    char  nr_expr[256]      = "x^2 - 2";
    float nr_x0             = 1.0f;
    float nr_tol            = 1e-6f;
    int   nr_max_iter       = 100;
    NRResult nr_result      = {};
    bool  nr_has_result     = false;
    bool  nr_show_table     = true;

    // Evaluateur
    char  ev_expr[256]      = "sin(x) + x^2";
    float ev_x              = 1.0f;
    float ev_result         = 0.0f;
    bool  ev_has_result     = false;
    bool  ev_show_ast       = true;
    Node* ev_ast            = nullptr;

    // Parametres
    char  param_hist[512]   = "history.csv";
};

static EtatApp g_etat;

// =============================================================================
//  THEME
// =============================================================================
static void AppliquerTheme() {
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding    = 0.0f;
    s.ChildRounding     = 8.0f;
    s.FrameRounding     = 6.0f;
    s.PopupRounding     = 6.0f;
    s.ScrollbarRounding = 6.0f;
    s.GrabRounding      = 4.0f;
    s.TabRounding       = 6.0f;
    s.WindowPadding     = {0.0f,  0.0f};
    s.FramePadding      = {10.0f, 7.0f};
    s.ItemSpacing       = {10.0f, 8.0f};
    s.ItemInnerSpacing  = {8.0f,  6.0f};
    s.ScrollbarSize     = 10.0f;
    s.GrabMinSize       = 10.0f;
    s.WindowBorderSize  = 0.0f;
    s.FrameBorderSize   = 0.0f;
    s.ChildBorderSize   = 0.0f;

    ImVec4* c = s.Colors;
    c[ImGuiCol_WindowBg]             = Theme::BgPanel;
    c[ImGuiCol_ChildBg]              = Theme::BgPanel;
    c[ImGuiCol_PopupBg]              = Theme::BgPanel;
    c[ImGuiCol_FrameBg]              = Theme::BgDeep;
    c[ImGuiCol_FrameBgHovered]       = Theme::BgCard;
    c[ImGuiCol_FrameBgActive]        = Theme::BgCard;
    c[ImGuiCol_TitleBg]              = Theme::BgDeep;
    c[ImGuiCol_TitleBgActive]        = Theme::BgDeep;
    c[ImGuiCol_TitleBgCollapsed]     = Theme::BgDeep;
    c[ImGuiCol_ScrollbarBg]          = Theme::BgDeep;
    c[ImGuiCol_ScrollbarGrab]        = Theme::AccentDim;
    c[ImGuiCol_ScrollbarGrabHovered] = Theme::Accent;
    c[ImGuiCol_ScrollbarGrabActive]  = Theme::AccentHover;
    c[ImGuiCol_CheckMark]            = Theme::Accent;
    c[ImGuiCol_SliderGrab]           = Theme::Accent;
    c[ImGuiCol_SliderGrabActive]     = Theme::AccentHover;
    c[ImGuiCol_Button]               = Theme::AccentDim;
    c[ImGuiCol_ButtonHovered]        = Theme::Accent;
    c[ImGuiCol_ButtonActive]         = Theme::AccentHover;
    c[ImGuiCol_Header]               = Theme::BgCard;
    c[ImGuiCol_HeaderHovered]        = {0.16f,0.19f,0.24f,1.0f};
    c[ImGuiCol_HeaderActive]         = Theme::AccentDim;
    c[ImGuiCol_Separator]            = Theme::Separator;
    c[ImGuiCol_SeparatorHovered]     = Theme::Accent;
    c[ImGuiCol_SeparatorActive]      = Theme::AccentHover;
    c[ImGuiCol_Tab]                  = Theme::BgPanel;
    c[ImGuiCol_TabHovered]           = Theme::Accent;
    c[ImGuiCol_TabActive]            = Theme::AccentDim;
    c[ImGuiCol_TabUnfocused]         = Theme::BgPanel;
    c[ImGuiCol_TabUnfocusedActive]   = Theme::BgCard;
    c[ImGuiCol_TableHeaderBg]        = {0.10f,0.12f,0.16f,1.0f};
    c[ImGuiCol_TableBorderLight]     = Theme::Separator;
    c[ImGuiCol_TableBorderStrong]    = Theme::Separator;
    c[ImGuiCol_TableRowBg]           = {0.00f,0.00f,0.00f,0.00f};
    c[ImGuiCol_TableRowBgAlt]        = {1.00f,1.00f,1.00f,0.03f};
    c[ImGuiCol_Text]                 = Theme::TextPrimary;
    c[ImGuiCol_TextDisabled]         = Theme::TextMuted;
    c[ImGuiCol_NavHighlight]         = Theme::Accent;
    c[ImGuiCol_ResizeGrip]           = Theme::AccentDim;
    c[ImGuiCol_ResizeGripHovered]    = Theme::Accent;
    c[ImGuiCol_ResizeGripActive]     = Theme::AccentHover;
}

// =============================================================================
//  WIDGETS UTILITAIRES
// =============================================================================
static bool BoutonAccent(const char* label, ImVec2 taille = {0, 36}) {
    ImGui::PushStyleColor(ImGuiCol_Button,        Theme::AccentDim);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Theme::Accent);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  Theme::AccentHover);
    bool c = ImGui::Button(label, taille);
    ImGui::PopStyleColor(3);
    return c;
}

static void Badge(const char* texte, ImVec4 couleur) {
    ImVec2 p  = ImGui::GetCursorScreenPos();
    ImVec2 sz = ImGui::CalcTextSize(texte);
    float  pd = 6.0f;
    ImGui::GetWindowDrawList()->AddRectFilled(
        {p.x - pd,        p.y - 2.0f},
        {p.x + sz.x + pd, p.y + sz.y + 2.0f},
        ImGui::ColorConvertFloat4ToU32(
            {couleur.x, couleur.y, couleur.z, 0.20f}), 4.0f);
    ImGui::TextColored(couleur, "%s", texte);
}

static void LigneParam(const char* libelle) {
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6.0f);
    ImGui::TextColored(Theme::TextMuted, "%s", libelle);
    ImGui::TableNextColumn();
}

// =============================================================================
//  ARBRE AST
// =============================================================================
struct NoeudVisu {
    std::string label;
    ImVec2      pos;
    int         gauche = -1;
    int         droite = -1;
};

static std::vector<NoeudVisu> g_ast_noeuds;

static const char* NomType(ParserTokenType t) {
    switch(t) {
        case NUM: return "NUM"; case VAR:  return "VAR";
        case FUNC:return "FN";  case OP1:  return "OP1";
        case OP2: return "OP2"; case ERR:  return "ERR";
        default:  return "?";
    }
}

static int ASTConstruire(Node* n, std::vector<NoeudVisu>& v) {
    if (!n || !n->self) return -1;
    NoeudVisu nd;
    char buf[48];
    if (n->self->type == NUM)
        snprintf(buf, sizeof(buf), "%.4g", n->self->n_val);
    else if (n->self->c_val)
        snprintf(buf, sizeof(buf), "%s", n->self->c_val);
    else
        snprintf(buf, sizeof(buf), "%s", NomType(n->self->type));
    nd.label = buf;
    int idx = (int)v.size();
    v.push_back(nd);
    v[idx].gauche = ASTConstruire(n->left,  v);
    v[idx].droite = ASTConstruire(n->right, v);
    return idx;
}

static float ASTLayout(int idx, std::vector<NoeudVisu>& v,
                        float x, float y) {
    if (idx < 0) return x;
    const float NW = 50.0f, NH = 72.0f;
    v[idx].pos.y = y;
    int g = v[idx].gauche, d = v[idx].droite;
    if (g < 0 && d < 0) { v[idx].pos.x = x; return x + NW + 8.0f; }
    float cur = x;
    float xg  = cur;
    if (g >= 0) cur = ASTLayout(g, v, cur, y + NH);
    float xd  = cur + 8.0f;
    if (d >= 0) cur = ASTLayout(d, v, xd,  y + NH);
    float ml = (g >= 0) ? v[g].pos.x : xg;
    float mr = (d >= 0) ? v[d].pos.x : xd;
    v[idx].pos.x = (ml + mr) * 0.5f;
    return cur;
}

static void ASTReconstruire(Node* racine) {
    g_ast_noeuds.clear();
    if (!racine) return;
    ASTConstruire(racine, g_ast_noeuds);
    ASTLayout(0, g_ast_noeuds, 20.0f, 20.0f);
}

static void RenderArbreAST(float largeur, float hauteur) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, {0.06f,0.07f,0.09f,1.0f});
    ImGui::BeginChild("##ast", {largeur, hauteur}, false,
        ImGuiWindowFlags_HorizontalScrollbar);

    ImVec2     origin = ImGui::GetCursorScreenPos();
    ImDrawList* dl    = ImGui::GetWindowDrawList();
    const float R     = 22.0f;
    ImU32 cNode  = ImGui::ColorConvertFloat4ToU32(Theme::BgCard);
    ImU32 cBord  = ImGui::ColorConvertFloat4ToU32(Theme::Accent);
    ImU32 cLine  = ImGui::ColorConvertFloat4ToU32(Theme::Separator);
    ImU32 cText  = ImGui::ColorConvertFloat4ToU32(Theme::TextPrimary);

    // Aretes
    for (int i = 0; i < (int)g_ast_noeuds.size(); i++) {
        auto& n  = g_ast_noeuds[i];
        ImVec2 pc = {origin.x + n.pos.x + R, origin.y + n.pos.y + R};
        auto arete = [&](int ei) {
            if (ei < 0) return;
            auto& e  = g_ast_noeuds[ei];
            ImVec2 pe = {origin.x + e.pos.x + R, origin.y + e.pos.y + R};
            ImVec2 c1 = {pc.x, (pc.y + pe.y) * 0.5f};
            ImVec2 c2 = {pe.x, (pc.y + pe.y) * 0.5f};
            dl->AddBezierCubic(pc, c1, c2, pe, cLine, 1.5f, 16);
        };
        arete(n.gauche); arete(n.droite);
    }
    // Noeuds
    for (auto& n : g_ast_noeuds) {
        ImVec2 pc = {origin.x + n.pos.x + R, origin.y + n.pos.y + R};
        dl->AddCircleFilled(pc, R,     cNode, 32);
        dl->AddCircle      (pc, R,     cBord, 32, 1.5f);
        ImVec2 ts = ImGui::CalcTextSize(n.label.c_str());
        dl->AddText({pc.x - ts.x*0.5f, pc.y - ts.y*0.5f}, cText,
                    n.label.c_str());
    }
    // Taille virtuelle pour le scroll
    if (!g_ast_noeuds.empty()) {
        float mx = 0, my = 0;
        for (auto& n : g_ast_noeuds) {
            if (n.pos.x + R*2 > mx) mx = n.pos.x + R*2;
            if (n.pos.y + R*2 > my) my = n.pos.y + R*2;
        }
        ImGui::Dummy({mx + 24.0f, my + 24.0f});
    }
    ImGui::EndChild();
    ImGui::PopStyleColor();
}

// =============================================================================
//  BARRE LATERALE
// =============================================================================
static void RenderBareLatérale(float w, float h) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::SidebarBg);
    ImGui::SetCursorPos({0, 0});
    ImGui::BeginChild("##sidebar", {w, h}, false);

    ImGui::SetCursorPos({20.0f, 22.0f});
    ImGui::TextColored(Theme::Accent, "ARITHMETICS");
    ImGui::SetCursorPos({20.0f, ImGui::GetCursorPosY()});
    ImGui::TextColored(Theme::TextMuted, "v1.0  —  Outils Numériques");
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 14.0f);
    ImGui::SetCursorPosX(20.0f);
    ImGui::Separator();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0f);
    ImGui::SetCursorPosX(20.0f);
    ImGui::TextColored(Theme::TextMuted, "NAVIGATION");
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6.0f);

    struct ItemNav { const char* label; Page page; };
    static ItemNav nav[] = {
        {"  Tableau de bord",     Page::Accueil           },
        {"  Méthodes Numériques", Page::MethodesNumeriques},
        {"  Évaluateur f(x)",     Page::EvaluateurFonction},
        {"  Paramètres",          Page::Parametres        },
    };
    for (auto& item : nav) {
        bool sel = (g_etat.page_courante == item.page);
        ImGui::PushStyleColor(ImGuiCol_Header,
            sel ? Theme::SidebarSel : ImVec4{0,0,0,0});
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, Theme::SidebarSel);
        ImGui::SetCursorPosX(8.0f);
        if (sel) ImGui::PushStyleColor(ImGuiCol_Text, Theme::Accent);
        bool clique = ImGui::Selectable(item.label, sel,
                        0, {w - 16.0f, 38.0f});
        if (sel) ImGui::PopStyleColor();
        ImGui::PopStyleColor(2);
        if (clique) g_etat.page_courante = item.page;
    }

    ImGui::SetCursorPosY(h - 56.0f);
    ImGui::SetCursorPosX(20.0f);
    ImGui::Separator();
    ImGui::SetCursorPosY(h - 44.0f);
    ImGui::SetCursorPosX(20.0f);
    ImGui::TextColored(Theme::TextMuted, "Faculté des Sciences");
    ImGui::SetCursorPosX(20.0f);
    ImGui::TextColored(Theme::TextMuted, "Projet INF448 — 2026");

    ImGui::EndChild();
    ImGui::PopStyleColor();
}

// =============================================================================
//  ACCUEIL — Tableau de bord
// =============================================================================
static void RenderAccueil(float cw, float ch) {
    const float GAP = 16.0f;

    ImGui::TextColored(Theme::TextPrimary, "Tableau de bord");
    ImGui::TextColored(Theme::TextMuted,
        "Sélectionnez un module pour démarrer un calcul.");
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 14.0f);
    ImGui::Separator();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20.0f);

    ImGui::TextColored(Theme::TextMuted, "MODULES DISPONIBLES");
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);

    float card_w = (cw - GAP) * 0.5f;
    float card_h = 110.0f;

    // Carte Méthodes Numériques
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::BgCard);
        ImGui::BeginChild("##c_mn", {card_w, card_h}, false);
        ImGui::SetCursorPos({16.0f, 14.0f});
        ImGui::TextColored(Theme::Accent, "Méthodes Numériques");
        ImGui::SetCursorPos({16.0f, 36.0f});
        ImGui::TextColored(Theme::TextMuted,
            "Recherche de racines par itération.");
        ImGui::SetCursorPos({16.0f, 54.0f});
        ImGui::TextColored(Theme::TextMuted, "Newton-Raphson");
        ImGui::SetCursorPos({16.0f, card_h - 32.0f});
        if (BoutonAccent("Ouvrir##mn", {card_w - 32.0f, 26.0f}))
            g_etat.page_courante = Page::MethodesNumeriques;
        ImGui::EndChild();
        ImGui::PopStyleColor();
    }
    ImGui::SameLine(0, GAP);
    // Carte Évaluateur f(x)
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::BgCard);
        ImGui::BeginChild("##c_ev", {card_w, card_h}, false);
        ImGui::SetCursorPos({16.0f, 14.0f});
        ImGui::TextColored(Theme::Accent, "Évaluateur f(x)");
        ImGui::SetCursorPos({16.0f, 36.0f});
        ImGui::TextColored(Theme::TextMuted,
            "Évaluation d'expressions mathématiques.");
        ImGui::SetCursorPos({16.0f, 54.0f});
        ImGui::TextColored(Theme::TextMuted, "Résultat + Arbre syntaxique AST");
        ImGui::SetCursorPos({16.0f, card_h - 32.0f});
        if (BoutonAccent("Ouvrir##ev", {card_w - 32.0f, 26.0f}))
            g_etat.page_courante = Page::EvaluateurFonction;
        ImGui::EndChild();
        ImGui::PopStyleColor();
    }

    // ── Historique ────────────────────────────────
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 22.0f);
    ImGui::TextColored(Theme::TextMuted, "HISTORIQUE DES OPÉRATIONS");
    ImGui::SameLine(cw - 72.0f);
    ImGui::PushStyleColor(ImGuiCol_Button,        {0.20f,0.08f,0.08f,1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Theme::Error);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  Theme::Error);
    if (ImGui::SmallButton("Effacer##h")) HistoriqueEffacer();
    ImGui::PopStyleColor(3);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0f);

    float hist_h = ch - ImGui::GetCursorPosY() - 4.0f;
    ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::BgCard);
    ImGui::BeginChild("##hist", {cw, hist_h}, false);

    if (g_historique.empty()) {
        float tw = ImGui::CalcTextSize(
            "Aucune opération enregistrée.").x;
        ImGui::SetCursorPos({(cw - tw)*0.5f, hist_h*0.42f});
        ImGui::TextColored(Theme::TextMuted,
            "Aucune opération enregistrée.");
    } else {
        if (ImGui::BeginTable("##ht", 3,
                ImGuiTableFlags_Borders    |
                ImGuiTableFlags_RowBg      |
                ImGuiTableFlags_ScrollY    |
                ImGuiTableFlags_SizingFixedFit,
                {cw, hist_h})) {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("Date / Heure",
                ImGuiTableColumnFlags_WidthFixed, 150.0f);
            ImGui::TableSetupColumn("Module",
                ImGuiTableColumnFlags_WidthFixed, 165.0f);
            ImGui::TableSetupColumn("Opération",
                ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();
            for (int i = (int)g_historique.size()-1; i >= 0; i--) {
                auto& e = g_historique[i];
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextColored(Theme::TextMuted, "%s", e.datetime);
                ImGui::TableNextColumn();
                ImGui::TextColored(Theme::Accent, "%s", e.module);
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(e.operation);
            }
            ImGui::EndTable();
        }
    }
    ImGui::EndChild();
    ImGui::PopStyleColor();
}

// =============================================================================
//  METHODES NUMERIQUES
// =============================================================================
static void RenderMethodesNumeriques(float cw, float ch) {
    const float GAP   = 16.0f;
    const float LW    = cw * 0.38f;
    const float RW    = cw - LW - GAP;
    const float COL_H = ch - 80.0f;
    const float IP    = 16.0f;

    ImGui::TextColored(Theme::TextPrimary, "Méthodes Numériques");
    ImGui::TextColored(Theme::TextMuted,
        "Recherche itérative de racines de fonctions réelles.");
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 12.0f);
    ImGui::Separator();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 16.0f);

    // ── Panneau Paramètres ────────────────────────
    ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::BgCard);
    ImGui::BeginChild("##nr_p", {LW, COL_H}, false);
    {
        ImGui::SetCursorPos({IP, IP});
        ImGui::TextColored(Theme::TextMuted, "PARAMÈTRES");
        ImGui::SetCursorPosX(IP);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6.0f);
        ImGui::Separator();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 12.0f);

        ImGui::SetCursorPosX(IP);
        ImGui::TextColored(Theme::TextMuted, "Méthode");
        ImGui::SetCursorPosX(IP);
        static int mi = 0;
        const char* mths[] = {"Newton-Raphson"};
        ImGui::SetNextItemWidth(LW - IP*2);
        ImGui::Combo("##mth", &mi, mths, IM_ARRAYSIZE(mths));
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);

        ImGui::SetCursorPosX(IP);
        ImGui::TextColored(Theme::TextMuted, "f(x)  —  expression en x");
        ImGui::SetCursorPosX(IP);
        ImGui::SetNextItemWidth(LW - IP*2);
        ImGui::InputText("##ne", g_etat.nr_expr, sizeof(g_etat.nr_expr));
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);

        ImGui::SetCursorPosX(IP);
        if (ImGui::BeginTable("##nrpt", 2, 0, {LW-IP*2, 0})) {
            ImGui::TableSetupColumn("l", ImGuiTableColumnFlags_WidthFixed, 130.0f);
            ImGui::TableSetupColumn("v", ImGuiTableColumnFlags_WidthStretch);
            LigneParam("x\u2080  (valeur initiale)");
            ImGui::SetNextItemWidth(-1);
            ImGui::InputFloat("##nx0", &g_etat.nr_x0, 0.1f,1.0f,"%.4f");
            LigneParam("Tolérance");
            ImGui::SetNextItemWidth(-1);
            ImGui::InputFloat("##ntol",&g_etat.nr_tol,0,0,"%.2e");
            LigneParam("Itérations max.");
            ImGui::SetNextItemWidth(-1);
            ImGui::InputInt("##nmx",&g_etat.nr_max_iter);
            if (g_etat.nr_max_iter < 1)   g_etat.nr_max_iter = 1;
            if (g_etat.nr_max_iter > 500) g_etat.nr_max_iter = 500;
            ImGui::EndTable();
        }

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 12.0f);
        ImGui::SetCursorPosX(IP);
        ImGui::TextColored(Theme::TextMuted, "Fonctions : sin cos tan log ln");
        ImGui::SetCursorPosX(IP);
        ImGui::TextColored(Theme::TextMuted, "Opérateurs : + - * / ^ ( )");
        ImGui::SetCursorPosX(IP);
        ImGui::TextColored(Theme::TextMuted, "Variable : x");

        ImGui::SetCursorPosY(COL_H - IP - 36.0f);
        ImGui::SetCursorPosX(IP);
        if (BoutonAccent("  Résoudre  ", {LW - IP*2, 36.0f})) {
            if (g_etat.nr_has_result) nr_free(&g_etat.nr_result);
            g_etat.nr_result     = newton_raphson(
                g_etat.nr_expr, g_etat.nr_x0,
                g_etat.nr_tol,  g_etat.nr_max_iter);
            g_etat.nr_has_result = true;
            char op[256];
            snprintf(op, sizeof(op),
                "newton-raphson \"%s\" ; x\u2080=%.4f ; tol=%.0e ; nmax=%d",
                g_etat.nr_expr, g_etat.nr_x0,
                (double)g_etat.nr_tol, g_etat.nr_max_iter);
            HistoriqueAjouter("Méthodes Numériques", op);
        }
    }
    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::SameLine(0, GAP);

    // ── Panneau Résultat ──────────────────────────
    ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::BgCard);
    ImGui::BeginChild("##nr_r", {RW, COL_H}, false);
    {
        ImGui::SetCursorPos({IP, IP});
        ImGui::TextColored(Theme::TextMuted, "RÉSULTAT");
        ImGui::SetCursorPosX(IP);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6.0f);
        ImGui::Separator();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 12.0f);

        if (!g_etat.nr_has_result) {
            const char* msg =
                "Entrez les paramètres puis cliquez Résoudre.";
            float tw = ImGui::CalcTextSize(msg).x;
            ImGui::SetCursorPosY(COL_H * 0.38f);
            ImGui::SetCursorPosX(IP + (RW - IP*2 - tw)*0.5f);
            ImGui::TextColored(Theme::TextMuted, "%s", msg);
        } else {
            NRResult* r = &g_etat.nr_result;
            ImGui::SetCursorPosX(IP);
            if (r->converged) {
                Badge("Convergé", Theme::Success);
                ImGui::SameLine();
                char buf[64];
                snprintf(buf,sizeof(buf),
                    "  Racine \u2248 %.8f", r->root);
                ImGui::TextColored(Theme::TextPrimary, "%s", buf);
                ImGui::SetCursorPosX(IP);
                ImGui::TextColored(Theme::TextMuted,
                    "%d itération(s)", r->iter_count);
            } else {
                Badge("Non convergé", Theme::Error);
                ImGui::SetCursorPosX(IP);
                ImGui::TextColored(Theme::TextMuted,
                    "Essayez un autre x\u2080 ou augmentez les itérations.");
            }
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
            ImGui::SetCursorPosX(IP);
            ImGui::Checkbox("Afficher le tableau d'itérations",
                &g_etat.nr_show_table);
            if (g_etat.nr_show_table) {
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0f);
                float th = COL_H - ImGui::GetCursorPosY() - IP;
                if (ImGui::BeginTable("##nrt", 6,
                        ImGuiTableFlags_Borders        |
                        ImGuiTableFlags_RowBg          |
                        ImGuiTableFlags_ScrollY        |
                        ImGuiTableFlags_SizingFixedFit |
                        ImGuiTableFlags_NoHostExtendX,
                        {RW - IP, th})) {
                    ImGui::TableSetupScrollFreeze(0,1);
                    ImGui::TableSetupColumn("n",
                        ImGuiTableColumnFlags_WidthFixed, 28.0f);
                    ImGui::TableSetupColumn("x_n",
                        ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("f(x_n)",
                        ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("f'(x_n)",
                        ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("x_(n+1)",
                        ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("delta_x",
                        ImGuiTableColumnFlags_WidthFixed, 68.0f);
                    ImGui::TableHeadersRow();
                    for (int i = 0; i < r->iter_count; i++) {
                        NRIteration* row = &r->iterations[i];
                        ImGui::TableNextRow();
                        if (i == r->iter_count-1 && r->converged)
                            ImGui::TableSetBgColor(
                                ImGuiTableBgTarget_RowBg0,
                                ImGui::ColorConvertFloat4ToU32(
                                    {Theme::Accent.x, Theme::Accent.y,
                                     Theme::Accent.z, 0.12f}));
                        ImGui::TableNextColumn();
                        ImGui::TextColored(Theme::TextMuted,"%d",row->n);
                        ImGui::TableNextColumn();
                        ImGui::Text("%.6f", row->x_n);
                        ImGui::TableNextColumn();
                        ImGui::Text("%.6f", row->fx_n);
                        ImGui::TableNextColumn();
                        ImGui::Text("%.6f", row->dfx_n);
                        ImGui::TableNextColumn();
                        ImGui::Text("%.6f", row->x_next);
                        ImGui::TableNextColumn();
                        ImVec4 dc = (row->delta_x < g_etat.nr_tol*10)
                            ? Theme::Success : Theme::TextPrimary;
                        ImGui::TextColored(dc,"%.2e",row->delta_x);
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
//  EVALUATEUR f(x)
// =============================================================================
static void RenderEvaluateurFonction(float cw, float ch) {
    const float GAP   = 16.0f;
    const float LW    = cw * 0.38f;
    const float RW    = cw - LW - GAP;
    const float COL_H = ch - 80.0f;
    const float IP    = 16.0f;

    ImGui::TextColored(Theme::TextPrimary, "Évaluateur f(x)");
    ImGui::TextColored(Theme::TextMuted,
        "Évalue une expression mathématique pour une valeur de x donnée.");
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 12.0f);
    ImGui::Separator();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 16.0f);

    // ── Panneau Paramètres ────────────────────────
    ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::BgCard);
    ImGui::BeginChild("##ev_p", {LW, COL_H}, false);
    {
        ImGui::SetCursorPos({IP, IP});
        ImGui::TextColored(Theme::TextMuted, "PARAMÈTRES");
        ImGui::SetCursorPosX(IP);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6.0f);
        ImGui::Separator();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 12.0f);

        ImGui::SetCursorPosX(IP);
        ImGui::TextColored(Theme::TextMuted, "f(x)  —  expression en x");
        ImGui::SetCursorPosX(IP);
        ImGui::SetNextItemWidth(LW - IP*2);
        ImGui::InputText("##ee", g_etat.ev_expr, sizeof(g_etat.ev_expr));
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);

        ImGui::SetCursorPosX(IP);
        if (ImGui::BeginTable("##evpt", 2, 0, {LW-IP*2, 0})) {
            ImGui::TableSetupColumn("l", ImGuiTableColumnFlags_WidthFixed, 130.0f);
            ImGui::TableSetupColumn("v", ImGuiTableColumnFlags_WidthStretch);
            LigneParam("Valeur de x");
            ImGui::SetNextItemWidth(-1);
            ImGui::InputFloat("##ex", &g_etat.ev_x, 0.1f,1.0f,"%.4f");
            ImGui::EndTable();
        }

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 12.0f);
        ImGui::SetCursorPosX(IP);
        ImGui::TextColored(Theme::TextMuted, "Fonctions : sin cos tan log ln");
        ImGui::SetCursorPosX(IP);
        ImGui::TextColored(Theme::TextMuted, "Opérateurs : + - * / ^ ( )");
        ImGui::SetCursorPosX(IP);
        ImGui::TextColored(Theme::TextMuted, "Variable : x");

        ImGui::SetCursorPosY(COL_H - IP - 36.0f);
        ImGui::SetCursorPosX(IP);
        if (BoutonAccent("  Évaluer  ", {LW - IP*2, 36.0f})) {
            if (g_etat.ev_ast) { free_tree(g_etat.ev_ast); g_etat.ev_ast=nullptr; }
            char buf[256];
            strncpy(buf, g_etat.ev_expr, sizeof(buf)-1);
            g_etat.ev_ast        = tokenize(buf);
            g_etat.ev_result     = evaluate_node(g_etat.ev_ast, g_etat.ev_x);
            g_etat.ev_has_result = true;
            ASTReconstruire(g_etat.ev_ast);
            char op[256];
            snprintf(op,sizeof(op),
                "eval \"%s\" ; x=%.4f  ->  %.6f",
                g_etat.ev_expr, g_etat.ev_x, g_etat.ev_result);
            HistoriqueAjouter("Évaluateur f(x)", op);
        }
    }
    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::SameLine(0, GAP);

    // ── Panneau Résultat + AST ────────────────────
    ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::BgCard);
    ImGui::BeginChild("##ev_r", {RW, COL_H}, false);
    {
        ImGui::SetCursorPos({IP, IP});
        ImGui::TextColored(Theme::TextMuted, "RÉSULTAT");
        ImGui::SetCursorPosX(IP);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6.0f);
        ImGui::Separator();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 12.0f);

        if (!g_etat.ev_has_result) {
            const char* msg =
                "Entrez une expression puis cliquez Évaluer.";
            float tw = ImGui::CalcTextSize(msg).x;
            ImGui::SetCursorPosY(COL_H * 0.38f);
            ImGui::SetCursorPosX(IP + (RW-IP*2-tw)*0.5f);
            ImGui::TextColored(Theme::TextMuted, "%s", msg);
        } else {
            bool err = (g_etat.ev_ast && g_etat.ev_ast->self &&
                        g_etat.ev_ast->self->type == ERR);
            ImGui::SetCursorPosX(IP);
            if (err) {
                Badge("Erreur de syntaxe", Theme::Error);
                ImGui::SetCursorPosX(IP);
                if (g_etat.ev_ast->self->c_val)
                    ImGui::TextColored(Theme::Error, "%s",
                        g_etat.ev_ast->self->c_val);
            } else if (isnan(g_etat.ev_result)) {
                Badge("Résultat indéfini", Theme::Warning);
                ImGui::SetCursorPosX(IP);
                ImGui::TextColored(Theme::TextMuted,
                    "Division par zéro ou domaine invalide.");
            } else {
                Badge("Évalué", Theme::Success);
                ImGui::SameLine();
                char buf[64];
                snprintf(buf,sizeof(buf),
                    "  f(%.4f) = %.8f", g_etat.ev_x, g_etat.ev_result);
                ImGui::TextColored(Theme::TextPrimary, "%s", buf);
            }
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 12.0f);
            ImGui::SetCursorPosX(IP);
            ImGui::Checkbox("Afficher l'arbre syntaxique (AST)",
                &g_etat.ev_show_ast);
            if (g_etat.ev_show_ast && !err) {
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0f);
                float ah = COL_H - ImGui::GetCursorPosY() - IP;
                RenderArbreAST(RW - IP, ah);
            }
        }
    }
    ImGui::EndChild();
    ImGui::PopStyleColor();
}

// =============================================================================
//  PARAMETRES
// =============================================================================
static void RenderParametres(float cw) {
    ImGui::TextColored(Theme::TextPrimary, "Paramètres");
    ImGui::TextColored(Theme::TextMuted, "Configuration de l'application.");
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 12.0f);
    ImGui::Separator();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20.0f);

    const float IP = 16.0f;

    ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::BgCard);
    ImGui::BeginChild("##p_hist", {cw, 112.0f}, false);
    {
        ImGui::SetCursorPos({IP, IP});
        ImGui::TextColored(Theme::TextMuted, "FICHIER D'HISTORIQUE");
        ImGui::SetCursorPosX(IP);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4.0f);
        ImGui::Separator();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4.0f);
        ImGui::SetCursorPosX(IP);
        ImGui::TextColored(Theme::TextMuted,
            "Chemin du fichier CSV d'historique");
        ImGui::SetCursorPosX(IP);
        ImGui::SetNextItemWidth(cw - IP*2 - 104.0f);
        ImGui::InputText("##hp", g_etat.param_hist, sizeof(g_etat.param_hist));
        ImGui::SameLine();
        if (BoutonAccent("Appliquer##hpa", {94.0f, 28.0f})) {
            strncpy(g_history_path, g_etat.param_hist,
                    sizeof(g_history_path)-1);
            HistoriqueCharger();
        }
    }
    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 16.0f);

    ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::BgCard);
    ImGui::BeginChild("##p_about", {cw, 122.0f}, false);
    {
        ImGui::SetCursorPos({IP, IP});
        ImGui::TextColored(Theme::TextMuted, "À PROPOS");
        ImGui::SetCursorPosX(IP);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0f);
        ImGui::Separator();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
        ImGui::SetCursorPosX(IP);
        ImGui::TextColored(Theme::TextPrimary, "Arithmetics  v1.0");
        ImGui::SetCursorPosX(IP);
        ImGui::TextColored(Theme::TextMuted,
            "Projet TPE INF448");
        // ImGui::SetCursorPosX(IP);
        // ImGui::TextColored(Theme::TextMuted,
        //     "Interface : Dear ImGui + SDL2 + OpenGL3");
        // ImGui::SetCursorPosX(IP);
        // ImGui::TextColored(Theme::TextMuted,
        //     "Moteur : C11  |  Parseur : math_parser  "
        //     "|  Méthodes : numeric_methods");
    }
    ImGui::EndChild();
    ImGui::PopStyleColor();
}

// =============================================================================
//  MAIN
// =============================================================================
int main() {

    HistoriqueCharger();

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init : %s\n", SDL_GetError());
        return 1;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,        0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,  1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,   24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,  8);

    SDL_Window* window = SDL_CreateWindow(
        "Projet TPE — Arithmetics",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 760,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
        SDL_WINDOW_ALLOW_HIGHDPI);
    if (!window) {
        fprintf(stderr,"SDL_CreateWindow : %s\n", SDL_GetError());
        return 1;
    }
    SDL_GLContext gl_ctx = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_ctx);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename  = nullptr;

    // Charger Footlight MT Light
    ImFontConfig cfg;
    cfg.OversampleH = 3; cfg.OversampleV = 2;
    ImFont* font_base = io.Fonts->AddFontFromFileTTF(
        "C:\\Windows\\Fonts\\FTLTLT.TTF",
        Theme::FontSizeBase, &cfg);
    if (!font_base) {
        io.Fonts->AddFontDefault();
        fprintf(stderr,
            "Avertissement : FTLTLT.TTF introuvable, "
            "police par défaut utilisée.\n");
    }
    // Police agrandie pour les titres de page
    ImFont* font_titre = io.Fonts->AddFontFromFileTTF(
        "C:\\Windows\\Fonts\\FTLTLT.TTF",
        Theme::FontSizeBase + 5.0f, &cfg);

    AppliquerTheme();
    ImGui_ImplSDL2_InitForOpenGL(window, gl_ctx);
    ImGui_ImplOpenGL3_Init("#version 150");

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(window))
                running = false;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Fenêtre hôte plein écran sans padding
        ImGui::SetNextWindowPos({0,0});
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0,0});
        ImGui::Begin("##host", nullptr,
            ImGuiWindowFlags_NoTitleBar       |
            ImGuiWindowFlags_NoResize         |
            ImGuiWindowFlags_NoMove           |
            ImGuiWindowFlags_NoScrollbar      |
            ImGuiWindowFlags_NoScrollWithMouse|
            ImGuiWindowFlags_NoBringToFrontOnFocus);
        ImGui::PopStyleVar();

        float ww  = io.DisplaySize.x;
        float wh  = io.DisplaySize.y;
        float sw  = 220.0f;     // largeur barre latérale
        float cp  = 28.0f;      // padding contenu uniforme
        float cw  = ww - sw - cp*2.0f;
        float ch  = wh - cp*2.0f;

        // Barre latérale — bord gauche, pleine hauteur, zéro offset
        RenderBareLatérale(sw, wh);

        // Zone de contenu — padding uniforme sur tous les côtés
        ImGui::SetCursorPos({sw + cp, cp});
        ImGui::BeginChild("##content", {cw, ch}, false);

        // Titre de page en grande police
        if (font_titre) ImGui::PushFont(font_titre);
        switch (g_etat.page_courante) {
            case Page::Accueil:
                if (font_titre) ImGui::PopFont();
                RenderAccueil(cw, ch);
                break;
            case Page::MethodesNumeriques:
                if (font_titre) ImGui::PopFont();
                RenderMethodesNumeriques(cw, ch);
                break;
            case Page::EvaluateurFonction:
                if (font_titre) ImGui::PopFont();
                RenderEvaluateurFonction(cw, ch);
                break;
            case Page::Parametres:
                if (font_titre) ImGui::PopFont();
                RenderParametres(cw);
                break;
            default:
                if (font_titre) ImGui::PopFont();
                break;
        }

        ImGui::EndChild();
        ImGui::End();

        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0.07f, 0.08f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    if (g_etat.nr_has_result) nr_free(&g_etat.nr_result);
    if (g_etat.ev_ast)        free_tree(g_etat.ev_ast);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl_ctx);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}