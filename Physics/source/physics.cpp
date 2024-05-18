#include <os/OSError.h>
#include <sy_core.h>
#include <modules.h>
#include <ft/fighter.h>
#include <st/st_utility.h>

namespace Physics {
    void aerialTransitionFix(Fighter* fighter) {
        soStatusModule* statusModule = fighter->m_moduleAccesser->getStatusModule();
        soPostureModule* postureModule = fighter->m_moduleAccesser->getPostureModule();
        soGroundModule* groundModule = fighter->m_moduleAccesser->getGroundModule();

        if (groundModule->getCollStatus(0)->m_touchFlag7 && ((statusModule->getPrevStatusKind(0) == 0x33 && statusModule->getStatusKind() == 0xE) || (statusModule->getPrevStatusKind(0) == 0x45 && statusModule->getStatusKind() == 0x49))) {
            Vec3f newPos = postureModule->getPos();
            groundModule->setCorrect(2, 0);
            fighter->processAnim();
            fighter->processUpdate();
            fighter->processPreMapCorrection;
            fighter->processMapCorrection();
            fighter->processFixPosition();
            postureModule->initPos(&newPos);
        }
    }

    void smoothWavedashes(Fighter* fighter) {
        soMotionModule* motionModule = fighter->m_moduleAccesser->getMotionModule();
        soGroundModule* groundModule = fighter->m_moduleAccesser->getGroundModule();
        soPostureModule* postureModule = fighter->m_moduleAccesser->getPostureModule();
        soStatusModule* statusModule = fighter->m_moduleAccesser->getStatusModule();
        soKineticModule* kineticModule = fighter->m_moduleAccesser->getKineticModule();

        if (statusModule->getStatusKind() == 0x21 &&
            motionModule->getFrame() == 0.0 &&
            statusModule->getPrevStatusKind(0) != 0x72 &&
            statusModule->getPrevStatusKind(0) != 0x7A &&
            kineticModule->getEnergy(0)->getSpeed().m_y < -0.001) {

            Vec2f downPos = groundModule->getDownPos(0);
            Vec3f startPos = {downPos.m_x, downPos.m_y, 0.0};
            Vec3f dir = {0.0, -1.0, 0.0};
            if (stRayCheck(&startPos, &dir, true, NULL, true, 1)) {
                groundModule->attachGround(0);
                groundModule->apply();
                fighter->processAnim();
                fighter->processUpdate();
                fighter->processPreMapCorrection();
                fighter->processMapCorrection;
                fighter->processFixPosition();

                downPos = groundModule->getDownPos(0);
                Vec3f outHitPos;
                Vec3f outCollNormalVec;
                if (stRayCheck(&(Vec3f){startPos.m_x, startPos.m_y, 0.0}, &(Vec3f){0.0, downPos.m_y - startPos.m_y, 0.0}, &outHitPos, &outCollNormalVec, true, NULL, true, 1)) {
                    Vec3f pos = postureModule->getPos();
                    pos.m_y = outHitPos.m_y;
                    postureModule->setPos(&pos);
                    groundModule->relocate(&outHitPos.m_xy, 0);
                }
                Vec2f rightPos = groundModule->getRightPos(0);
                Vec2f leftPos = groundModule->getLeftPos(0);
                if (stRayCheck(&(Vec3f){rightPos.m_x, rightPos.m_y, 0.0}, &(Vec3f){-5.0, 0.0, 0.0}, true, NULL, true, 1) ||
                    stRayCheck(&(Vec3f){leftPos.m_x, leftPos.m_y, 0.0}, &(Vec3f){5.0, 0.0, 0.0}, true, NULL, true, 1)) {
                    Vec3f prevPos = postureModule->getPrevPos();
                    Vec3f pos = postureModule->getPos();
                    pos.m_x = prevPos.m_x;
                    postureModule->setPos(&pos);
                }
            }






        }

    }

    void instantFastfall(Fighter* fighter) {

    }

    void fastfallTumble(Fighter* fighter) {

    }

    void gameplayFixes(Fighter* fighter) {
        aerialTransitionFix(fighter);
        smoothWavedashes(fighter);
        instantFastfall(fighter);
        fastfallTumble(fighter);
    }

    asm void postFighterProcessFixPosition()
    {
        nofralloc // don't need stack frame
        mr r3, r26
        bl gameplayFixes
        psq_l f31,0x68(r1),0,0  // original operation
    }

    void Init()
    {
        OSReport("Hello World\n");

        SyringeCore::sySimpleHookRel(0x12F998, reinterpret_cast<void*>(postFighterProcessFixPosition), Modules::SORA_MELEE);
    }

    void Destroy()
    {
        OSReport("Goodbye\n");
    }
}