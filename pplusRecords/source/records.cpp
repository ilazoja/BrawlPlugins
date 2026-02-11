#include <os/OSError.h>
#include <plugin.hpp>
#include <hook.hpp>
#include <modules.h>
#include <ft/fighter.h>
#include <st/st_utility.h>
#include <nt/network.h>
#include <mu/selchar/mu_selchar_player_area.h>
#include <VF.h>
#include <cstdio>
#include <cstdlib>
#include <mu/menu.h>
#include <sc/sc_sel_char.h>
#include "net_log.h"

#define REQUEST_BUFFER_SIZE 0x1000
char requestBuffer[REQUEST_BUFFER_SIZE];
const char baseUrl[] = "http://brawl.gs.nwfc.wiinoma.com";

NHTTPResponseHandle lastResponse = NULL;

namespace Records {

    void HandleResponse(NHTTPError, NHTTPResponseHandle response, void*) {
        if (lastResponse != NULL) {
            NHTTPDestroyResponse(lastResponse);
        }
        lastResponse = response;

        if (NHTTPGetResultCode(response) == 200) {
            scSelChar* selCharScene = static_cast<scSelChar*>(gfSceneManager::getInstance()->searchScene("scSelctCharacter"));
            if (selCharScene->m_selCharMenu != NULL) {
                char* responseBuffer;
                char* contentLength;
                NHTTPGetBodyAll(response, &responseBuffer);
                selCharScene->m_selCharMenu->dispHighScore(2, atoi(responseBuffer), 1);
            }
        }

    }

    void InitNetwork()
    {
        SOInitInfo info = {
            SOAlloc,
            SOFree
        };
        SOInit(&info);
        SOStartupEx(180000);

        DWC_Init(1, "smashbrosxwii", 0x5253424a, nteDWC::AllocEx, nteDWC::FreeEx);
        if (NHTTPStartup(DWCi_NdAlloc, DWCi_NdFree, 0x11) == 0) {

        }
    }

    void PostRecord() {

    }

    void SendCharacterRequest() {

        register int mucharKind;
        register int level;

        asm {
            mr mucharKind, r3
            mr level, r30
        }

        char url[100];
        sprintf(url, "%s/target?name=%s&character=%d&level=%d", baseUrl, "test", mucharKind, level);

        NHTTPRequestHandle request = NHTTPCreateRequest(url, NHTTP_REQMETHOD_GET, requestBuffer, REQUEST_BUFFER_SIZE, HandleResponse, NULL);
        NHTTPSendRequestAsync(request);
    }

    void Init(Plugin* api)
    {
        //g_Network.create(true);

//        InitNetwork();
//        NetLog::Init();

        //OSReport("TESTTTTTTTTTT\

        api->addHookEx(0x00000094, reinterpret_cast<void*>(InitNetwork), SyringeCore::OPT_ORIG_PRE | SyringeCore::OPT_SAVE_REGS, Modules::SORA_MENU_SEL_CHAR);
        api->addHookEx(0x0000B318, reinterpret_cast<void*>(SendCharacterRequest), SyringeCore::OPT_ORIG_PRE | SyringeCore::OPT_SAVE_REGS, Modules::SORA_MENU_SEL_CHAR);
        api->addHookEx(0x00252D18, reinterpret_cast<void*>(PostRecord), SyringeCore::OPT_ORIG_PRE | SyringeCore::OPT_SAVE_REGS, Modules::SORA_MELEE);
    }

    void Destroy()
    {
       // OSReport("Goodbye\n");
    }
}