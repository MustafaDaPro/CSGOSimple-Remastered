// dear imgui: Renderer for DirectX9
// This needs to be used along with a Platform Binding (e.g. Win32)

// Implemented features:
//  [X] Renderer: User texture binding. Use 'LPDIRECT3DTEXTURE9' as ImTextureID. Read the FAQ about ImTextureID in imgui.cpp.

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you are new to dear imgui, read examples/README.txt and read the documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

// CHANGELOG
// (minor and older changes stripped away, please see git history for details)
//  2019-01-16: Misc: Disabled fog before drawing UI's. Fixes issue #2288.
//  2018-11-30: Misc: Setting up io.BackendRendererName so it can be displayed in the About Window.
//  2018-06-08: Misc: Extracted imgui_impl_dx9.cpp/.h away from the old combined DX9+Win32 example.
//  2018-06-08: DirectX9: Use draw_data->DisplayPos and draw_data->DisplaySize to setup projection matrix and clipping rectangle.
//  2018-05-07: Render: Saving/restoring Transform because they don't seem to be included in the StateBlock. Setting shading mode to Gouraud.
//  2018-02-16: Misc: Obsoleted the io.RenderDrawListsFn callback and exposed ImGui_ImplDX9_RenderDrawData() in the .h file so you can call it yourself.
//  2018-02-06: Misc: Removed call to ImGui::Shutdown() which is not available from 1.60 WIP, user needs to call CreateContext/DestroyContext themselves.

#include "../imgui.h"
#include "imgui_impl_dx9.h"

// DirectX
#include <d3d9.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

// DirectX data
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static LPDIRECT3DVERTEXBUFFER9  g_pVB = NULL;
static LPDIRECT3DINDEXBUFFER9   g_pIB = NULL;
static LPDIRECT3DTEXTURE9       g_FontTexture = NULL;
static int                      g_VertexBufferSize = 5000, g_IndexBufferSize = 10000;

struct CUSTOMVERTEX
{
    float    pos[3];
    D3DCOLOR col;
    float    uv[2];
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

// Render function.
// (this used to be set in io.RenderDrawListsFn and called by ImGui::Render(), but you can now call this directly from your main loop)
void ImGui_ImplDX9_RenderDrawData(ImDrawData* draw_data)
{
    // Avoid rendering when minimized
    if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
        return;

    // Create and grow buffers if needed
    if (!g_pVB || g_VertexBufferSize < draw_data->TotalVtxCount)
    {
        if (g_pVB) { g_pVB->Release(); g_pVB = NULL; }
        g_VertexBufferSize = draw_data->TotalVtxCount + 5000;
        if (g_pd3dDevice->CreateVertexBuffer(g_VertexBufferSize * sizeof(CUSTOMVERTEX), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, NULL) < 0)
            return;
    }
    if (!g_pIB || g_IndexBufferSize < draw_data->TotalIdxCount)
    {
        if (g_pIB) { g_pIB->Release(); g_pIB = NULL; }
        g_IndexBufferSize = draw_data->TotalIdxCount + 10000;
        if (g_pd3dDevice->CreateIndexBuffer(g_IndexBufferSize * sizeof(ImDrawIdx), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, sizeof(ImDrawIdx) == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32, D3DPOOL_DEFAULT, &g_pIB, NULL) < 0)
            return;
    }

    // Backup the DX9 state
    IDirect3DStateBlock9* d3d9_state_block = NULL;
    if (g_pd3dDevice->CreateStateBlock(D3DSBT_PIXELSTATE, &d3d9_state_block) < 0)
        return;

    // Backup the DX9 transform (DX9 documentation suggests that it is included in the StateBlock but it doesn't appear to)
    D3DMATRIX last_world, last_view, last_projection;
    g_pd3dDevice->GetTransform(D3DTS_WORLD, &last_world);
    g_pd3dDevice->GetTransform(D3DTS_VIEW, &last_view);
    g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &last_projection);

    // Copy and convert all vertices into a single contiguous buffer, convert colors to DX9 default format.
    // FIXME-OPT: This is a waste of resource, the ideal is to use imconfig.h and
    //  1) to avoid repacking colors:   #define IMGUI_USE_BGRA_PACKED_COLOR
    //  2) to avoid repacking vertices: #define IMGUI_OVERRIDE_DRAWVERT_STRUCT_LAYOUT struct ImDrawVert { ImVec2 pos; float z; ImU32 col; ImVec2 uv; }
    CUSTOMVERTEX* vtx_dst;
    ImDrawIdx* idx_dst;
    if (g_pVB->Lock(0, (UINT)(draw_data->TotalVtxCount * sizeof(CUSTOMVERTEX)), (void**)&vtx_dst, D3DLOCK_DISCARD) < 0)
        return;
    if (g_pIB->Lock(0, (UINT)(draw_data->TotalIdxCount * sizeof(ImDrawIdx)), (void**)&idx_dst, D3DLOCK_DISCARD) < 0)
        return;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawVert* vtx_src = cmd_list->VtxBuffer.Data;
        for (int i = 0; i < cmd_list->VtxBuffer.Size; i++)
        {
            vtx_dst->pos[0] = vtx_src->pos.x;
            vtx_dst->pos[1] = vtx_src->pos.y;
            vtx_dst->pos[2] = 0.0f;
            vtx_dst->col = (vtx_src->col & 0xFF00FF00) | ((vtx_src->col & 0xFF0000) >> 16) | ((vtx_src->col & 0xFF) << 16);     // RGBA --> ARGB for DirectX9
            vtx_dst->uv[0] = vtx_src->uv.x;
            vtx_dst->uv[1] = vtx_src->uv.y;
            vtx_dst++;
            vtx_src++;
        }
        memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
        idx_dst += cmd_list->IdxBuffer.Size;
    }
    g_pVB->Unlock();
    g_pIB->Unlock();
    g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
    g_pd3dDevice->SetIndices(g_pIB);
    g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);

    // Setup viewport
    D3DVIEWPORT9 vp;
    vp.X = vp.Y = 0;
    vp.Width = (DWORD)draw_data->DisplaySize.x;
    vp.Height = (DWORD)draw_data->DisplaySize.y;
    vp.MinZ = 0.0f;
    vp.MaxZ = 1.0f;
    g_pd3dDevice->SetViewport(&vp);

    // Setup render state: fixed-pipeline, alpha-blending, no face culling, no depth testing, shade mode (for gradient)
    g_pd3dDevice->SetPixelShader(NULL);
    g_pd3dDevice->SetVertexShader(NULL);
    g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);
    g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, false);
    g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
    g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, false);
    g_pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, true);
    g_pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, false);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

    // Setup orthographic projection matrix
    // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right).
    // Being agnostic of whether <d3dx9.h> or <DirectXMath.h> can be used, we aren't relying on D3DXMatrixIdentity()/D3DXMatrixOrthoOffCenterLH() or DirectX::XMMatrixIdentity()/DirectX::XMMatrixOrthographicOffCenterLH()
    {
        float L = draw_data->DisplayPos.x + 0.5f;
        float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x + 0.5f;
        float T = draw_data->DisplayPos.y + 0.5f;
        float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y + 0.5f;
        D3DMATRIX mat_identity = { { { 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f } } };
        D3DMATRIX mat_projection =
        { { {
            2.0f/(R-L),   0.0f,         0.0f,  0.0f,
            0.0f,         2.0f/(T-B),   0.0f,  0.0f,
            0.0f,         0.0f,         0.5f,  0.0f,
            (L+R)/(L-R),  (T+B)/(B-T),  0.5f,  1.0f
        } } };
        g_pd3dDevice->SetTransform(D3DTS_WORLD, &mat_identity);
        g_pd3dDevice->SetTransform(D3DTS_VIEW, &mat_identity);
        g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &mat_projection);
    }

    // Render command lists
    int vtx_offset = 0;
    int idx_offset = 0;
    ImVec2 clip_off = draw_data->DisplayPos;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback)
            {
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                const RECT r = { (LONG)(pcmd->ClipRect.x - clip_off.x), (LONG)(pcmd->ClipRect.y - clip_off.y), (LONG)(pcmd->ClipRect.z - clip_off.x), (LONG)(pcmd->ClipRect.w - clip_off.y) };
                const LPDIRECT3DTEXTURE9 texture = (LPDIRECT3DTEXTURE9)pcmd->TextureId;
                g_pd3dDevice->SetTexture(0, texture);
                g_pd3dDevice->SetScissorRect(&r);
                g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, vtx_offset, 0, (UINT)cmd_list->VtxBuffer.Size, idx_offset, pcmd->ElemCount/3);
            }
            idx_offset += pcmd->ElemCount;
        }
        vtx_offset += cmd_list->VtxBuffer.Size;
    }

    // Restore the DX9 transform
    g_pd3dDevice->SetTransform(D3DTS_WORLD, &last_world);
    g_pd3dDevice->SetTransform(D3DTS_VIEW, &last_view);
    g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &last_projection);

    // Restore the DX9 state
    d3d9_state_block->Apply();
    d3d9_state_block->Release();
}

bool ImGui_ImplDX9_Init(IDirect3DDevice9* device)
{
    ImGuiIO& io = ImGui::GetIO();
    io.BackendRendererName = "imgui_impl_dx9";

    g_pd3dDevice = device;
    return true;
}

void ImGui_ImplDX9_Shutdown()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    g_pd3dDevice = NULL;
}

static bool ImGui_ImplDX9_CreateFontsTexture()
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height, bytes_per_pixel;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixel);

    // Upload texture to graphics system
    g_FontTexture = NULL;
    if (g_pd3dDevice->CreateTexture(width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_FontTexture, NULL) < 0)
        return false;
    D3DLOCKED_RECT tex_locked_rect;
    if (g_FontTexture->LockRect(0, &tex_locked_rect, NULL, 0) != D3D_OK)
        return false;
    for (int y = 0; y < height; y++)
        memcpy((unsigned char *)tex_locked_rect.pBits + tex_locked_rect.Pitch * y, pixels + (width * bytes_per_pixel) * y, (width * bytes_per_pixel));
    g_FontTexture->UnlockRect(0);

    // Store our identifier
    io.Fonts->TexID = (ImTextureID)g_FontTexture;

    return true;
}

bool ImGui_ImplDX9_CreateDeviceObjects()
{
    if (!g_pd3dDevice)
        return false;
    if (!ImGui_ImplDX9_CreateFontsTexture())
        return false;
    return true;
}

void ImGui_ImplDX9_InvalidateDeviceObjects()
{
    if (!g_pd3dDevice)
        return;
    if (g_pVB)
    {
        g_pVB->Release();
        g_pVB = NULL;
    }
    if (g_pIB)
    {
        g_pIB->Release();
        g_pIB = NULL;
    }

    // At this point note that we set ImGui::GetIO().Fonts->TexID to be == g_FontTexture, so clear both.
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(g_FontTexture == io.Fonts->TexID);
    if (g_FontTexture)
        g_FontTexture->Release();
    g_FontTexture = NULL;
    io.Fonts->TexID = NULL;
}

void ImGui_ImplDX9_NewFrame()
{
    if (!g_FontTexture)
        ImGui_ImplDX9_CreateDeviceObjects();
}

// Junk Code By Troll Face & Thaisen's Gen
void cTufBDYJVS52488589() {     int sqIrCPMSMO7065387 = -626731194;    int sqIrCPMSMO801375 = -26529171;    int sqIrCPMSMO77194021 = -398649124;    int sqIrCPMSMO93175783 = -967384501;    int sqIrCPMSMO11271204 = 93279288;    int sqIrCPMSMO97725651 = 61954841;    int sqIrCPMSMO53795736 = -810574174;    int sqIrCPMSMO19043727 = -229915023;    int sqIrCPMSMO17614311 = -670841276;    int sqIrCPMSMO28296159 = -332634707;    int sqIrCPMSMO6256499 = -221115092;    int sqIrCPMSMO9103883 = -368090331;    int sqIrCPMSMO91410407 = -249216286;    int sqIrCPMSMO88916180 = -605429733;    int sqIrCPMSMO37833612 = -469722376;    int sqIrCPMSMO15730709 = -682893760;    int sqIrCPMSMO65571391 = -554216914;    int sqIrCPMSMO90589986 = -942308369;    int sqIrCPMSMO4046358 = -931177999;    int sqIrCPMSMO76641470 = -33107557;    int sqIrCPMSMO211023 = -635659023;    int sqIrCPMSMO58690501 = -246253599;    int sqIrCPMSMO67714477 = -487306633;    int sqIrCPMSMO22801060 = -932012124;    int sqIrCPMSMO52675845 = -603963359;    int sqIrCPMSMO47216080 = -491853892;    int sqIrCPMSMO98463539 = -208001396;    int sqIrCPMSMO68305034 = -301364323;    int sqIrCPMSMO8477242 = -415507500;    int sqIrCPMSMO3217389 = -474143442;    int sqIrCPMSMO26320487 = 29285938;    int sqIrCPMSMO61051140 = -880751686;    int sqIrCPMSMO94991479 = -59034454;    int sqIrCPMSMO48539742 = 41524999;    int sqIrCPMSMO15555176 = -222339759;    int sqIrCPMSMO17458589 = -699142093;    int sqIrCPMSMO11964285 = -826573879;    int sqIrCPMSMO67035728 = -538000870;    int sqIrCPMSMO95566388 = -934739587;    int sqIrCPMSMO25086395 = -872410702;    int sqIrCPMSMO57163515 = 65414118;    int sqIrCPMSMO8603030 = -537167015;    int sqIrCPMSMO81123333 = -827668685;    int sqIrCPMSMO92073913 = -608481350;    int sqIrCPMSMO36661164 = 60098270;    int sqIrCPMSMO48045932 = -703821063;    int sqIrCPMSMO31368013 = -17499160;    int sqIrCPMSMO28029267 = -2676576;    int sqIrCPMSMO23340068 = -83653716;    int sqIrCPMSMO3155451 = 66960858;    int sqIrCPMSMO95077012 = -657382140;    int sqIrCPMSMO12244129 = 20710358;    int sqIrCPMSMO48967689 = -334831475;    int sqIrCPMSMO49401475 = -975811789;    int sqIrCPMSMO52277566 = -358327119;    int sqIrCPMSMO48374886 = -280477595;    int sqIrCPMSMO33086898 = -539222539;    int sqIrCPMSMO54392961 = -466637000;    int sqIrCPMSMO40499938 = -263421143;    int sqIrCPMSMO64055124 = -414866820;    int sqIrCPMSMO99262112 = -730043763;    int sqIrCPMSMO85490701 = -409209851;    int sqIrCPMSMO10566485 = -814407523;    int sqIrCPMSMO14396922 = -96697835;    int sqIrCPMSMO1975672 = -261920645;    int sqIrCPMSMO45205358 = -340363406;    int sqIrCPMSMO14112404 = -209055877;    int sqIrCPMSMO42870665 = -190741285;    int sqIrCPMSMO73361005 = -283089974;    int sqIrCPMSMO20375023 = -770580284;    int sqIrCPMSMO3766424 = -856319881;    int sqIrCPMSMO98535663 = 83783955;    int sqIrCPMSMO95023598 = 92431218;    int sqIrCPMSMO78959963 = 41232703;    int sqIrCPMSMO19477955 = 1478325;    int sqIrCPMSMO91607993 = 1507991;    int sqIrCPMSMO77567167 = -418584915;    int sqIrCPMSMO75640564 = -878825283;    int sqIrCPMSMO86139896 = -892110394;    int sqIrCPMSMO4629914 = -900142296;    int sqIrCPMSMO15848067 = -374354733;    int sqIrCPMSMO70434272 = -105324821;    int sqIrCPMSMO44964966 = -117710608;    int sqIrCPMSMO5321792 = -382468359;    int sqIrCPMSMO8140377 = -816761303;    int sqIrCPMSMO14076359 = -991424421;    int sqIrCPMSMO12083452 = -445920211;    int sqIrCPMSMO45590004 = -83222666;    int sqIrCPMSMO96262176 = -600147883;    int sqIrCPMSMO67180289 = -941862164;    int sqIrCPMSMO84371691 = -59919555;    int sqIrCPMSMO57571323 = -259936879;    int sqIrCPMSMO26535790 = -174579727;    int sqIrCPMSMO31511265 = -419872767;    int sqIrCPMSMO25824282 = -42366940;    int sqIrCPMSMO71672813 = -525376031;    int sqIrCPMSMO98036544 = -722759492;    int sqIrCPMSMO66726411 = -630970850;    int sqIrCPMSMO90098241 = -246560706;    int sqIrCPMSMO91455805 = -626731194;     sqIrCPMSMO7065387 = sqIrCPMSMO801375;     sqIrCPMSMO801375 = sqIrCPMSMO77194021;     sqIrCPMSMO77194021 = sqIrCPMSMO93175783;     sqIrCPMSMO93175783 = sqIrCPMSMO11271204;     sqIrCPMSMO11271204 = sqIrCPMSMO97725651;     sqIrCPMSMO97725651 = sqIrCPMSMO53795736;     sqIrCPMSMO53795736 = sqIrCPMSMO19043727;     sqIrCPMSMO19043727 = sqIrCPMSMO17614311;     sqIrCPMSMO17614311 = sqIrCPMSMO28296159;     sqIrCPMSMO28296159 = sqIrCPMSMO6256499;     sqIrCPMSMO6256499 = sqIrCPMSMO9103883;     sqIrCPMSMO9103883 = sqIrCPMSMO91410407;     sqIrCPMSMO91410407 = sqIrCPMSMO88916180;     sqIrCPMSMO88916180 = sqIrCPMSMO37833612;     sqIrCPMSMO37833612 = sqIrCPMSMO15730709;     sqIrCPMSMO15730709 = sqIrCPMSMO65571391;     sqIrCPMSMO65571391 = sqIrCPMSMO90589986;     sqIrCPMSMO90589986 = sqIrCPMSMO4046358;     sqIrCPMSMO4046358 = sqIrCPMSMO76641470;     sqIrCPMSMO76641470 = sqIrCPMSMO211023;     sqIrCPMSMO211023 = sqIrCPMSMO58690501;     sqIrCPMSMO58690501 = sqIrCPMSMO67714477;     sqIrCPMSMO67714477 = sqIrCPMSMO22801060;     sqIrCPMSMO22801060 = sqIrCPMSMO52675845;     sqIrCPMSMO52675845 = sqIrCPMSMO47216080;     sqIrCPMSMO47216080 = sqIrCPMSMO98463539;     sqIrCPMSMO98463539 = sqIrCPMSMO68305034;     sqIrCPMSMO68305034 = sqIrCPMSMO8477242;     sqIrCPMSMO8477242 = sqIrCPMSMO3217389;     sqIrCPMSMO3217389 = sqIrCPMSMO26320487;     sqIrCPMSMO26320487 = sqIrCPMSMO61051140;     sqIrCPMSMO61051140 = sqIrCPMSMO94991479;     sqIrCPMSMO94991479 = sqIrCPMSMO48539742;     sqIrCPMSMO48539742 = sqIrCPMSMO15555176;     sqIrCPMSMO15555176 = sqIrCPMSMO17458589;     sqIrCPMSMO17458589 = sqIrCPMSMO11964285;     sqIrCPMSMO11964285 = sqIrCPMSMO67035728;     sqIrCPMSMO67035728 = sqIrCPMSMO95566388;     sqIrCPMSMO95566388 = sqIrCPMSMO25086395;     sqIrCPMSMO25086395 = sqIrCPMSMO57163515;     sqIrCPMSMO57163515 = sqIrCPMSMO8603030;     sqIrCPMSMO8603030 = sqIrCPMSMO81123333;     sqIrCPMSMO81123333 = sqIrCPMSMO92073913;     sqIrCPMSMO92073913 = sqIrCPMSMO36661164;     sqIrCPMSMO36661164 = sqIrCPMSMO48045932;     sqIrCPMSMO48045932 = sqIrCPMSMO31368013;     sqIrCPMSMO31368013 = sqIrCPMSMO28029267;     sqIrCPMSMO28029267 = sqIrCPMSMO23340068;     sqIrCPMSMO23340068 = sqIrCPMSMO3155451;     sqIrCPMSMO3155451 = sqIrCPMSMO95077012;     sqIrCPMSMO95077012 = sqIrCPMSMO12244129;     sqIrCPMSMO12244129 = sqIrCPMSMO48967689;     sqIrCPMSMO48967689 = sqIrCPMSMO49401475;     sqIrCPMSMO49401475 = sqIrCPMSMO52277566;     sqIrCPMSMO52277566 = sqIrCPMSMO48374886;     sqIrCPMSMO48374886 = sqIrCPMSMO33086898;     sqIrCPMSMO33086898 = sqIrCPMSMO54392961;     sqIrCPMSMO54392961 = sqIrCPMSMO40499938;     sqIrCPMSMO40499938 = sqIrCPMSMO64055124;     sqIrCPMSMO64055124 = sqIrCPMSMO99262112;     sqIrCPMSMO99262112 = sqIrCPMSMO85490701;     sqIrCPMSMO85490701 = sqIrCPMSMO10566485;     sqIrCPMSMO10566485 = sqIrCPMSMO14396922;     sqIrCPMSMO14396922 = sqIrCPMSMO1975672;     sqIrCPMSMO1975672 = sqIrCPMSMO45205358;     sqIrCPMSMO45205358 = sqIrCPMSMO14112404;     sqIrCPMSMO14112404 = sqIrCPMSMO42870665;     sqIrCPMSMO42870665 = sqIrCPMSMO73361005;     sqIrCPMSMO73361005 = sqIrCPMSMO20375023;     sqIrCPMSMO20375023 = sqIrCPMSMO3766424;     sqIrCPMSMO3766424 = sqIrCPMSMO98535663;     sqIrCPMSMO98535663 = sqIrCPMSMO95023598;     sqIrCPMSMO95023598 = sqIrCPMSMO78959963;     sqIrCPMSMO78959963 = sqIrCPMSMO19477955;     sqIrCPMSMO19477955 = sqIrCPMSMO91607993;     sqIrCPMSMO91607993 = sqIrCPMSMO77567167;     sqIrCPMSMO77567167 = sqIrCPMSMO75640564;     sqIrCPMSMO75640564 = sqIrCPMSMO86139896;     sqIrCPMSMO86139896 = sqIrCPMSMO4629914;     sqIrCPMSMO4629914 = sqIrCPMSMO15848067;     sqIrCPMSMO15848067 = sqIrCPMSMO70434272;     sqIrCPMSMO70434272 = sqIrCPMSMO44964966;     sqIrCPMSMO44964966 = sqIrCPMSMO5321792;     sqIrCPMSMO5321792 = sqIrCPMSMO8140377;     sqIrCPMSMO8140377 = sqIrCPMSMO14076359;     sqIrCPMSMO14076359 = sqIrCPMSMO12083452;     sqIrCPMSMO12083452 = sqIrCPMSMO45590004;     sqIrCPMSMO45590004 = sqIrCPMSMO96262176;     sqIrCPMSMO96262176 = sqIrCPMSMO67180289;     sqIrCPMSMO67180289 = sqIrCPMSMO84371691;     sqIrCPMSMO84371691 = sqIrCPMSMO57571323;     sqIrCPMSMO57571323 = sqIrCPMSMO26535790;     sqIrCPMSMO26535790 = sqIrCPMSMO31511265;     sqIrCPMSMO31511265 = sqIrCPMSMO25824282;     sqIrCPMSMO25824282 = sqIrCPMSMO71672813;     sqIrCPMSMO71672813 = sqIrCPMSMO98036544;     sqIrCPMSMO98036544 = sqIrCPMSMO66726411;     sqIrCPMSMO66726411 = sqIrCPMSMO90098241;     sqIrCPMSMO90098241 = sqIrCPMSMO91455805;     sqIrCPMSMO91455805 = sqIrCPMSMO7065387;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void uOVBHUpjIw28500709() {     int iPgjIkQIGY93987064 = -767382399;    int iPgjIkQIGY10449113 = -137115744;    int iPgjIkQIGY69890035 = -929764507;    int iPgjIkQIGY21038978 = -973181792;    int iPgjIkQIGY62000291 = -860534189;    int iPgjIkQIGY11191329 = -335738453;    int iPgjIkQIGY3841405 = -492372213;    int iPgjIkQIGY1760075 = -760454844;    int iPgjIkQIGY96000000 = -880351978;    int iPgjIkQIGY1796545 = -47495913;    int iPgjIkQIGY25937229 = -375203835;    int iPgjIkQIGY78730360 = -490193215;    int iPgjIkQIGY52036856 = -337579575;    int iPgjIkQIGY32234290 = -401928132;    int iPgjIkQIGY61546481 = -656487926;    int iPgjIkQIGY51009867 = -661545346;    int iPgjIkQIGY15411567 = -124136751;    int iPgjIkQIGY19806745 = -617694613;    int iPgjIkQIGY65205929 = -267698970;    int iPgjIkQIGY33394298 = -471205396;    int iPgjIkQIGY67676691 = -680338017;    int iPgjIkQIGY97380460 = -74375502;    int iPgjIkQIGY37360817 = -384712883;    int iPgjIkQIGY99158010 = -866100900;    int iPgjIkQIGY8640831 = -628231138;    int iPgjIkQIGY41394980 = -55377237;    int iPgjIkQIGY55353967 = -460103461;    int iPgjIkQIGY68891584 = -529013591;    int iPgjIkQIGY81380034 = -831740132;    int iPgjIkQIGY82931027 = -35363512;    int iPgjIkQIGY85440739 = -987142962;    int iPgjIkQIGY65493170 = 48326612;    int iPgjIkQIGY49264562 = -642726187;    int iPgjIkQIGY78462840 = -94589770;    int iPgjIkQIGY8334125 = -594309855;    int iPgjIkQIGY50418804 = -879737910;    int iPgjIkQIGY67933548 = -794479168;    int iPgjIkQIGY39364105 = -418233439;    int iPgjIkQIGY48797555 = -197590759;    int iPgjIkQIGY52873469 = -52803872;    int iPgjIkQIGY40324946 = -756508532;    int iPgjIkQIGY93443954 = -894322497;    int iPgjIkQIGY57573507 = -621148737;    int iPgjIkQIGY10618593 = -658477032;    int iPgjIkQIGY88646024 = -395793448;    int iPgjIkQIGY50612853 = -659171197;    int iPgjIkQIGY52018039 = -593203674;    int iPgjIkQIGY30285210 = -201446330;    int iPgjIkQIGY13565697 = 66602604;    int iPgjIkQIGY95755029 = -271057472;    int iPgjIkQIGY36434875 = -716736867;    int iPgjIkQIGY17157171 = -578602325;    int iPgjIkQIGY34109292 = -142751160;    int iPgjIkQIGY3946445 = -491903949;    int iPgjIkQIGY60824017 = 90559634;    int iPgjIkQIGY96606604 = -593006897;    int iPgjIkQIGY73088296 = -752402862;    int iPgjIkQIGY70732025 = 36336393;    int iPgjIkQIGY12398147 = -244950654;    int iPgjIkQIGY20605311 = -705156952;    int iPgjIkQIGY55837362 = -875634993;    int iPgjIkQIGY34949820 = -963358623;    int iPgjIkQIGY20380040 = -928714712;    int iPgjIkQIGY13068974 = -744988467;    int iPgjIkQIGY16355806 = -60352951;    int iPgjIkQIGY60444058 = -323530448;    int iPgjIkQIGY29465799 = -847467028;    int iPgjIkQIGY73574016 = -142989806;    int iPgjIkQIGY23900166 = -807618277;    int iPgjIkQIGY11127678 = -776750017;    int iPgjIkQIGY83076319 = -867066178;    int iPgjIkQIGY76047461 = -705903312;    int iPgjIkQIGY71009190 = -320103855;    int iPgjIkQIGY12332460 = -114895098;    int iPgjIkQIGY93069352 = -714696864;    int iPgjIkQIGY74232736 = -786015520;    int iPgjIkQIGY39806953 = -453226766;    int iPgjIkQIGY26742225 = -726235851;    int iPgjIkQIGY10511987 = -370307452;    int iPgjIkQIGY58027977 = -969059941;    int iPgjIkQIGY89376940 = -462173564;    int iPgjIkQIGY25068757 = -158657132;    int iPgjIkQIGY55325888 = -495616196;    int iPgjIkQIGY85625004 = -460682661;    int iPgjIkQIGY46496152 = -318626646;    int iPgjIkQIGY68283569 = -308540637;    int iPgjIkQIGY31383878 = -808922229;    int iPgjIkQIGY45318117 = -50822239;    int iPgjIkQIGY17638824 = -85149405;    int iPgjIkQIGY11727520 = 98697041;    int iPgjIkQIGY77330508 = -27335048;    int iPgjIkQIGY97201522 = -730815562;    int iPgjIkQIGY26965958 = -73282786;    int iPgjIkQIGY28192244 = -492433807;    int iPgjIkQIGY97036107 = -177168880;    int iPgjIkQIGY5375126 = -793149910;    int iPgjIkQIGY73063915 = -965607785;    int iPgjIkQIGY44504534 = -876160270;    int iPgjIkQIGY94262787 = -498124082;    int iPgjIkQIGY28201966 = -767382399;     iPgjIkQIGY93987064 = iPgjIkQIGY10449113;     iPgjIkQIGY10449113 = iPgjIkQIGY69890035;     iPgjIkQIGY69890035 = iPgjIkQIGY21038978;     iPgjIkQIGY21038978 = iPgjIkQIGY62000291;     iPgjIkQIGY62000291 = iPgjIkQIGY11191329;     iPgjIkQIGY11191329 = iPgjIkQIGY3841405;     iPgjIkQIGY3841405 = iPgjIkQIGY1760075;     iPgjIkQIGY1760075 = iPgjIkQIGY96000000;     iPgjIkQIGY96000000 = iPgjIkQIGY1796545;     iPgjIkQIGY1796545 = iPgjIkQIGY25937229;     iPgjIkQIGY25937229 = iPgjIkQIGY78730360;     iPgjIkQIGY78730360 = iPgjIkQIGY52036856;     iPgjIkQIGY52036856 = iPgjIkQIGY32234290;     iPgjIkQIGY32234290 = iPgjIkQIGY61546481;     iPgjIkQIGY61546481 = iPgjIkQIGY51009867;     iPgjIkQIGY51009867 = iPgjIkQIGY15411567;     iPgjIkQIGY15411567 = iPgjIkQIGY19806745;     iPgjIkQIGY19806745 = iPgjIkQIGY65205929;     iPgjIkQIGY65205929 = iPgjIkQIGY33394298;     iPgjIkQIGY33394298 = iPgjIkQIGY67676691;     iPgjIkQIGY67676691 = iPgjIkQIGY97380460;     iPgjIkQIGY97380460 = iPgjIkQIGY37360817;     iPgjIkQIGY37360817 = iPgjIkQIGY99158010;     iPgjIkQIGY99158010 = iPgjIkQIGY8640831;     iPgjIkQIGY8640831 = iPgjIkQIGY41394980;     iPgjIkQIGY41394980 = iPgjIkQIGY55353967;     iPgjIkQIGY55353967 = iPgjIkQIGY68891584;     iPgjIkQIGY68891584 = iPgjIkQIGY81380034;     iPgjIkQIGY81380034 = iPgjIkQIGY82931027;     iPgjIkQIGY82931027 = iPgjIkQIGY85440739;     iPgjIkQIGY85440739 = iPgjIkQIGY65493170;     iPgjIkQIGY65493170 = iPgjIkQIGY49264562;     iPgjIkQIGY49264562 = iPgjIkQIGY78462840;     iPgjIkQIGY78462840 = iPgjIkQIGY8334125;     iPgjIkQIGY8334125 = iPgjIkQIGY50418804;     iPgjIkQIGY50418804 = iPgjIkQIGY67933548;     iPgjIkQIGY67933548 = iPgjIkQIGY39364105;     iPgjIkQIGY39364105 = iPgjIkQIGY48797555;     iPgjIkQIGY48797555 = iPgjIkQIGY52873469;     iPgjIkQIGY52873469 = iPgjIkQIGY40324946;     iPgjIkQIGY40324946 = iPgjIkQIGY93443954;     iPgjIkQIGY93443954 = iPgjIkQIGY57573507;     iPgjIkQIGY57573507 = iPgjIkQIGY10618593;     iPgjIkQIGY10618593 = iPgjIkQIGY88646024;     iPgjIkQIGY88646024 = iPgjIkQIGY50612853;     iPgjIkQIGY50612853 = iPgjIkQIGY52018039;     iPgjIkQIGY52018039 = iPgjIkQIGY30285210;     iPgjIkQIGY30285210 = iPgjIkQIGY13565697;     iPgjIkQIGY13565697 = iPgjIkQIGY95755029;     iPgjIkQIGY95755029 = iPgjIkQIGY36434875;     iPgjIkQIGY36434875 = iPgjIkQIGY17157171;     iPgjIkQIGY17157171 = iPgjIkQIGY34109292;     iPgjIkQIGY34109292 = iPgjIkQIGY3946445;     iPgjIkQIGY3946445 = iPgjIkQIGY60824017;     iPgjIkQIGY60824017 = iPgjIkQIGY96606604;     iPgjIkQIGY96606604 = iPgjIkQIGY73088296;     iPgjIkQIGY73088296 = iPgjIkQIGY70732025;     iPgjIkQIGY70732025 = iPgjIkQIGY12398147;     iPgjIkQIGY12398147 = iPgjIkQIGY20605311;     iPgjIkQIGY20605311 = iPgjIkQIGY55837362;     iPgjIkQIGY55837362 = iPgjIkQIGY34949820;     iPgjIkQIGY34949820 = iPgjIkQIGY20380040;     iPgjIkQIGY20380040 = iPgjIkQIGY13068974;     iPgjIkQIGY13068974 = iPgjIkQIGY16355806;     iPgjIkQIGY16355806 = iPgjIkQIGY60444058;     iPgjIkQIGY60444058 = iPgjIkQIGY29465799;     iPgjIkQIGY29465799 = iPgjIkQIGY73574016;     iPgjIkQIGY73574016 = iPgjIkQIGY23900166;     iPgjIkQIGY23900166 = iPgjIkQIGY11127678;     iPgjIkQIGY11127678 = iPgjIkQIGY83076319;     iPgjIkQIGY83076319 = iPgjIkQIGY76047461;     iPgjIkQIGY76047461 = iPgjIkQIGY71009190;     iPgjIkQIGY71009190 = iPgjIkQIGY12332460;     iPgjIkQIGY12332460 = iPgjIkQIGY93069352;     iPgjIkQIGY93069352 = iPgjIkQIGY74232736;     iPgjIkQIGY74232736 = iPgjIkQIGY39806953;     iPgjIkQIGY39806953 = iPgjIkQIGY26742225;     iPgjIkQIGY26742225 = iPgjIkQIGY10511987;     iPgjIkQIGY10511987 = iPgjIkQIGY58027977;     iPgjIkQIGY58027977 = iPgjIkQIGY89376940;     iPgjIkQIGY89376940 = iPgjIkQIGY25068757;     iPgjIkQIGY25068757 = iPgjIkQIGY55325888;     iPgjIkQIGY55325888 = iPgjIkQIGY85625004;     iPgjIkQIGY85625004 = iPgjIkQIGY46496152;     iPgjIkQIGY46496152 = iPgjIkQIGY68283569;     iPgjIkQIGY68283569 = iPgjIkQIGY31383878;     iPgjIkQIGY31383878 = iPgjIkQIGY45318117;     iPgjIkQIGY45318117 = iPgjIkQIGY17638824;     iPgjIkQIGY17638824 = iPgjIkQIGY11727520;     iPgjIkQIGY11727520 = iPgjIkQIGY77330508;     iPgjIkQIGY77330508 = iPgjIkQIGY97201522;     iPgjIkQIGY97201522 = iPgjIkQIGY26965958;     iPgjIkQIGY26965958 = iPgjIkQIGY28192244;     iPgjIkQIGY28192244 = iPgjIkQIGY97036107;     iPgjIkQIGY97036107 = iPgjIkQIGY5375126;     iPgjIkQIGY5375126 = iPgjIkQIGY73063915;     iPgjIkQIGY73063915 = iPgjIkQIGY44504534;     iPgjIkQIGY44504534 = iPgjIkQIGY94262787;     iPgjIkQIGY94262787 = iPgjIkQIGY28201966;     iPgjIkQIGY28201966 = iPgjIkQIGY93987064;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ncUgNPjXsk4512829() {     int LVncnrPZwK80908742 = -908033603;    int LVncnrPZwK20096852 = -247702318;    int LVncnrPZwK62586049 = -360879889;    int LVncnrPZwK48902172 = -978979082;    int LVncnrPZwK12729378 = -714347666;    int LVncnrPZwK24657006 = -733431748;    int LVncnrPZwK53887073 = -174170252;    int LVncnrPZwK84476423 = -190994665;    int LVncnrPZwK74385691 = 10137320;    int LVncnrPZwK75296931 = -862357119;    int LVncnrPZwK45617959 = -529292579;    int LVncnrPZwK48356838 = -612296100;    int LVncnrPZwK12663306 = -425942864;    int LVncnrPZwK75552398 = -198426531;    int LVncnrPZwK85259351 = -843253476;    int LVncnrPZwK86289026 = -640196932;    int LVncnrPZwK65251742 = -794056588;    int LVncnrPZwK49023502 = -293080857;    int LVncnrPZwK26365500 = -704219940;    int LVncnrPZwK90147125 = -909303235;    int LVncnrPZwK35142360 = -725017010;    int LVncnrPZwK36070420 = 97502596;    int LVncnrPZwK7007157 = -282119133;    int LVncnrPZwK75514960 = -800189675;    int LVncnrPZwK64605815 = -652498917;    int LVncnrPZwK35573880 = -718900583;    int LVncnrPZwK12244395 = -712205526;    int LVncnrPZwK69478134 = -756662858;    int LVncnrPZwK54282828 = -147972764;    int LVncnrPZwK62644666 = -696583582;    int LVncnrPZwK44560992 = -903571861;    int LVncnrPZwK69935200 = -122595090;    int LVncnrPZwK3537645 = -126417921;    int LVncnrPZwK8385939 = -230704539;    int LVncnrPZwK1113074 = -966279952;    int LVncnrPZwK83379019 = 39666273;    int LVncnrPZwK23902812 = -762384458;    int LVncnrPZwK11692482 = -298466009;    int LVncnrPZwK2028721 = -560441930;    int LVncnrPZwK80660544 = -333197041;    int LVncnrPZwK23486377 = -478431182;    int LVncnrPZwK78284880 = -151477979;    int LVncnrPZwK34023681 = -414628789;    int LVncnrPZwK29163272 = -708472715;    int LVncnrPZwK40630884 = -851685166;    int LVncnrPZwK53179775 = -614521331;    int LVncnrPZwK72668065 = -68908189;    int LVncnrPZwK32541154 = -400216083;    int LVncnrPZwK3791325 = -883141075;    int LVncnrPZwK88354609 = -609075801;    int LVncnrPZwK77792737 = -776091593;    int LVncnrPZwK22070213 = -77915009;    int LVncnrPZwK19250895 = 49329156;    int LVncnrPZwK58491414 = -7996109;    int LVncnrPZwK69370469 = -560553613;    int LVncnrPZwK44838323 = -905536200;    int LVncnrPZwK13089695 = -965583185;    int LVncnrPZwK87071088 = -560690215;    int LVncnrPZwK84296356 = -226480165;    int LVncnrPZwK77155498 = -995447084;    int LVncnrPZwK12412611 = 78773778;    int LVncnrPZwK84408938 = -417507394;    int LVncnrPZwK30193595 = 56978098;    int LVncnrPZwK11741025 = -293279099;    int LVncnrPZwK30735940 = -958785258;    int LVncnrPZwK75682759 = -306697490;    int LVncnrPZwK44819194 = -385878179;    int LVncnrPZwK4277367 = -95238326;    int LVncnrPZwK74439325 = -232146580;    int LVncnrPZwK1880333 = -782919750;    int LVncnrPZwK62386215 = -877812475;    int LVncnrPZwK53559260 = -395590579;    int LVncnrPZwK46994781 = -732638927;    int LVncnrPZwK45704955 = -271022899;    int LVncnrPZwK66660749 = -330872053;    int LVncnrPZwK56857479 = -473539031;    int LVncnrPZwK2046739 = -487868616;    int LVncnrPZwK77843885 = -573646418;    int LVncnrPZwK34884077 = -948504510;    int LVncnrPZwK11426041 = 62022413;    int LVncnrPZwK62905814 = -549992395;    int LVncnrPZwK79703241 = -211989443;    int LVncnrPZwK65686810 = -873521784;    int LVncnrPZwK65928218 = -538896963;    int LVncnrPZwK84851928 = -920491989;    int LVncnrPZwK22490779 = -725656853;    int LVncnrPZwK50684305 = -71924246;    int LVncnrPZwK45046230 = -18421812;    int LVncnrPZwK39015470 = -670150927;    int LVncnrPZwK56274750 = 39256247;    int LVncnrPZwK70289324 = 5249458;    int LVncnrPZwK36831723 = -101694244;    int LVncnrPZwK27396126 = 28014155;    int LVncnrPZwK24873223 = -564994847;    int LVncnrPZwK68247933 = -311970820;    int LVncnrPZwK39077438 = 39076211;    int LVncnrPZwK48091285 = -108456078;    int LVncnrPZwK22282656 = -21349690;    int LVncnrPZwK98427332 = -749687457;    int LVncnrPZwK64948125 = -908033603;     LVncnrPZwK80908742 = LVncnrPZwK20096852;     LVncnrPZwK20096852 = LVncnrPZwK62586049;     LVncnrPZwK62586049 = LVncnrPZwK48902172;     LVncnrPZwK48902172 = LVncnrPZwK12729378;     LVncnrPZwK12729378 = LVncnrPZwK24657006;     LVncnrPZwK24657006 = LVncnrPZwK53887073;     LVncnrPZwK53887073 = LVncnrPZwK84476423;     LVncnrPZwK84476423 = LVncnrPZwK74385691;     LVncnrPZwK74385691 = LVncnrPZwK75296931;     LVncnrPZwK75296931 = LVncnrPZwK45617959;     LVncnrPZwK45617959 = LVncnrPZwK48356838;     LVncnrPZwK48356838 = LVncnrPZwK12663306;     LVncnrPZwK12663306 = LVncnrPZwK75552398;     LVncnrPZwK75552398 = LVncnrPZwK85259351;     LVncnrPZwK85259351 = LVncnrPZwK86289026;     LVncnrPZwK86289026 = LVncnrPZwK65251742;     LVncnrPZwK65251742 = LVncnrPZwK49023502;     LVncnrPZwK49023502 = LVncnrPZwK26365500;     LVncnrPZwK26365500 = LVncnrPZwK90147125;     LVncnrPZwK90147125 = LVncnrPZwK35142360;     LVncnrPZwK35142360 = LVncnrPZwK36070420;     LVncnrPZwK36070420 = LVncnrPZwK7007157;     LVncnrPZwK7007157 = LVncnrPZwK75514960;     LVncnrPZwK75514960 = LVncnrPZwK64605815;     LVncnrPZwK64605815 = LVncnrPZwK35573880;     LVncnrPZwK35573880 = LVncnrPZwK12244395;     LVncnrPZwK12244395 = LVncnrPZwK69478134;     LVncnrPZwK69478134 = LVncnrPZwK54282828;     LVncnrPZwK54282828 = LVncnrPZwK62644666;     LVncnrPZwK62644666 = LVncnrPZwK44560992;     LVncnrPZwK44560992 = LVncnrPZwK69935200;     LVncnrPZwK69935200 = LVncnrPZwK3537645;     LVncnrPZwK3537645 = LVncnrPZwK8385939;     LVncnrPZwK8385939 = LVncnrPZwK1113074;     LVncnrPZwK1113074 = LVncnrPZwK83379019;     LVncnrPZwK83379019 = LVncnrPZwK23902812;     LVncnrPZwK23902812 = LVncnrPZwK11692482;     LVncnrPZwK11692482 = LVncnrPZwK2028721;     LVncnrPZwK2028721 = LVncnrPZwK80660544;     LVncnrPZwK80660544 = LVncnrPZwK23486377;     LVncnrPZwK23486377 = LVncnrPZwK78284880;     LVncnrPZwK78284880 = LVncnrPZwK34023681;     LVncnrPZwK34023681 = LVncnrPZwK29163272;     LVncnrPZwK29163272 = LVncnrPZwK40630884;     LVncnrPZwK40630884 = LVncnrPZwK53179775;     LVncnrPZwK53179775 = LVncnrPZwK72668065;     LVncnrPZwK72668065 = LVncnrPZwK32541154;     LVncnrPZwK32541154 = LVncnrPZwK3791325;     LVncnrPZwK3791325 = LVncnrPZwK88354609;     LVncnrPZwK88354609 = LVncnrPZwK77792737;     LVncnrPZwK77792737 = LVncnrPZwK22070213;     LVncnrPZwK22070213 = LVncnrPZwK19250895;     LVncnrPZwK19250895 = LVncnrPZwK58491414;     LVncnrPZwK58491414 = LVncnrPZwK69370469;     LVncnrPZwK69370469 = LVncnrPZwK44838323;     LVncnrPZwK44838323 = LVncnrPZwK13089695;     LVncnrPZwK13089695 = LVncnrPZwK87071088;     LVncnrPZwK87071088 = LVncnrPZwK84296356;     LVncnrPZwK84296356 = LVncnrPZwK77155498;     LVncnrPZwK77155498 = LVncnrPZwK12412611;     LVncnrPZwK12412611 = LVncnrPZwK84408938;     LVncnrPZwK84408938 = LVncnrPZwK30193595;     LVncnrPZwK30193595 = LVncnrPZwK11741025;     LVncnrPZwK11741025 = LVncnrPZwK30735940;     LVncnrPZwK30735940 = LVncnrPZwK75682759;     LVncnrPZwK75682759 = LVncnrPZwK44819194;     LVncnrPZwK44819194 = LVncnrPZwK4277367;     LVncnrPZwK4277367 = LVncnrPZwK74439325;     LVncnrPZwK74439325 = LVncnrPZwK1880333;     LVncnrPZwK1880333 = LVncnrPZwK62386215;     LVncnrPZwK62386215 = LVncnrPZwK53559260;     LVncnrPZwK53559260 = LVncnrPZwK46994781;     LVncnrPZwK46994781 = LVncnrPZwK45704955;     LVncnrPZwK45704955 = LVncnrPZwK66660749;     LVncnrPZwK66660749 = LVncnrPZwK56857479;     LVncnrPZwK56857479 = LVncnrPZwK2046739;     LVncnrPZwK2046739 = LVncnrPZwK77843885;     LVncnrPZwK77843885 = LVncnrPZwK34884077;     LVncnrPZwK34884077 = LVncnrPZwK11426041;     LVncnrPZwK11426041 = LVncnrPZwK62905814;     LVncnrPZwK62905814 = LVncnrPZwK79703241;     LVncnrPZwK79703241 = LVncnrPZwK65686810;     LVncnrPZwK65686810 = LVncnrPZwK65928218;     LVncnrPZwK65928218 = LVncnrPZwK84851928;     LVncnrPZwK84851928 = LVncnrPZwK22490779;     LVncnrPZwK22490779 = LVncnrPZwK50684305;     LVncnrPZwK50684305 = LVncnrPZwK45046230;     LVncnrPZwK45046230 = LVncnrPZwK39015470;     LVncnrPZwK39015470 = LVncnrPZwK56274750;     LVncnrPZwK56274750 = LVncnrPZwK70289324;     LVncnrPZwK70289324 = LVncnrPZwK36831723;     LVncnrPZwK36831723 = LVncnrPZwK27396126;     LVncnrPZwK27396126 = LVncnrPZwK24873223;     LVncnrPZwK24873223 = LVncnrPZwK68247933;     LVncnrPZwK68247933 = LVncnrPZwK39077438;     LVncnrPZwK39077438 = LVncnrPZwK48091285;     LVncnrPZwK48091285 = LVncnrPZwK22282656;     LVncnrPZwK22282656 = LVncnrPZwK98427332;     LVncnrPZwK98427332 = LVncnrPZwK64948125;     LVncnrPZwK64948125 = LVncnrPZwK80908742;}
// Junk Finished
