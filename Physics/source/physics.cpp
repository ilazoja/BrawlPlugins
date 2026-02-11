#include <ft/fighter.h>
#include <modules.h>
#include <os/OSError.h>
#include <st/st_utility.h>
#include <hook.hpp>
#include <plugin.hpp>
#include <sy_core.hpp>

namespace Physics {
    void aerialTransitionFix(Fighter* fighter)
    {
        soStatusModule& statusModule = fighter->m_moduleAccesser->getStatusModule();
        soPostureModule& postureModule = fighter->m_moduleAccesser->getPostureModule();
        soGroundModule& groundModule = fighter->m_moduleAccesser->getGroundModule();
        grCollStatus* collStatus = groundModule.getCollStatus(0);

        if (collStatus != NULL && collStatus->m_touchFlag7 && ((statusModule.getPrevStatusKind(0) == Fighter::Status::Attack_Air && statusModule.getStatusKind() == Fighter::Status::Fall) || (statusModule.getPrevStatusKind(0) == Fighter::Status::Damage_Fly && statusModule.getStatusKind() == Fighter::Status::Damage_Fall)))
        {
            Vec3f newPos = postureModule.getPos();
            groundModule.setCorrect(soGroundShapeImpl::Correct_Ground_Cliff_Stop, 0);
            fighter->processAnim();
            fighter->processUpdate();
            fighter->processPreMapCorrection();
            fighter->processMapCorrection();
            fighter->processFixPosition();
            postureModule.initPos(&newPos);
        }
    }

    void smoothWavedashes(Fighter* fighter)
    {
        soMotionModule& motionModule = fighter->m_moduleAccesser->getMotionModule();
        soGroundModule& groundModule = fighter->m_moduleAccesser->getGroundModule();
        soPostureModule& postureModule = fighter->m_moduleAccesser->getPostureModule();
        soStatusModule& statusModule = fighter->m_moduleAccesser->getStatusModule();
        soKineticModule& kineticModule = fighter->m_moduleAccesser->getKineticModule();

        if (statusModule.getStatusKind() == Fighter::Status::Escape_Air &&
            motionModule.getFrame() == 0.0 &&
            statusModule.getPrevStatusKind(0) != Fighter::Status::Pass &&
            statusModule.getPrevStatusKind(0) != Fighter::Status::Cliff_Jump2 &&
            kineticModule.getEnergy(0)->getSpeed().m_y < -0.001)
        {

            Vec2f downPos = groundModule.getDownPos(0);
            Vec3f startPos(downPos.m_x, downPos.m_y, 0.0);
            Vec3f dir(0.0, -1.0, 0.0);
            if (stRayCheck(&startPos, &dir, true, NULL, true, 1))
            {
                groundModule.attachGround(0);
                groundModule.apply();
                fighter->processAnim();
                fighter->processUpdate();
                fighter->processPreMapCorrection();
                fighter->processMapCorrection();
                fighter->processFixPosition();

                downPos = groundModule.getDownPos(0);
                Vec3f outHitPos;
                Vec3f outCollNormalVec;

                dir = Vec3f( 0.0, downPos.m_y - startPos.m_y, 0.0);
                if (stRayCheck(&startPos, &dir, &outHitPos, &outCollNormalVec, true, NULL, true, 1))
                {
                    Vec3f pos = postureModule.getPos();
                    pos.m_y = outHitPos.m_y;
                    postureModule.setPos(&pos);
                    groundModule.relocate(outHitPos.xy(), 0);
                }
                Vec2f rightPos = groundModule.getRightPos(0);
                Vec2f leftPos = groundModule.getLeftPos(0);
                Vec3f rightPos3f = Vec3f(rightPos.m_x, rightPos.m_y, 0.0);
                Vec3f leftPos3f = Vec3f(leftPos.m_x, leftPos.m_y, 0.0);
                Vec3f leftDir = Vec3f(-5.0, 0.0, 0.0);
                Vec3f rightDir = Vec3f(5.0, 0.0, 0.0);

                if (stRayCheck(&rightPos3f, &leftDir, true, NULL, true, 1) ||
                    stRayCheck(&leftPos3f, &rightDir, true, NULL, true, 1))
                {
                    Vec3f prevPos = postureModule.getPrevPos();
                    Vec3f pos = postureModule.getPos();
                    pos.m_x = prevPos.m_x;
                    postureModule.setPos(&pos);
                }
            }
        }
    }

    void instantFastfall(Fighter* fighter)
    {
        soKineticModule& kineticModule = fighter->m_moduleAccesser->getKineticModule();
        soPostureModule& postureModule = fighter->m_moduleAccesser->getPostureModule();
        soWorkManageModule& workManageModule = fighter->m_moduleAccesser->getWorkManageModule();

        if (workManageModule.isFlag(0x20000000 | 0x02000000 | 0x2))
        {
            float prevSpeed = kineticModule.getEnergy(1)->getSpeed().m_y;
            kineticModule.getEnergy(1)->updateEnergy(fighter->m_moduleAccesser);
            float speed = kineticModule.getEnergy(1)->getSpeed().m_y;
            if (prevSpeed != speed)
            {
                Vec3f pos = postureModule.getPos();
                pos.m_y -= prevSpeed;
                postureModule.setPos(&pos);
                fighter->updatePosture(true);
                fighter->updateRoughPos();
                fighter->updateNodeSRT();
            }
        }
    }

    void fastfallTumble(Fighter* fighter)
    {
    }

    void gameplayFixes(Fighter* fighter)
    {
        aerialTransitionFix(fighter);
        smoothWavedashes(fighter);
        instantFastfall(fighter);
        fastfallTumble(fighter);
    }

    void postFighterProcessFixPosition()
    {
        register Fighter* fighter;

        asm { 
            mr fighter, r26
        }

        aerialTransitionFix(fighter);
        smoothWavedashes(fighter);
        instantFastfall(fighter);
        fastfallTumble(fighter);
    }

    void Init(Plugin* api)
    {
        // OSReport("Hello World\n");

        api->addHookEx(0x12F998, reinterpret_cast<void*>(postFighterProcessFixPosition),  SyringeCore::OPT_ORIG_PRE | SyringeCore::OPT_SAVE_REGS,Modules::SORA_MELEE);
    }

    void Destroy()
    {
        // OSReport("Goodbye\n");
    }
}