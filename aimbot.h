#ifndef __AIMBOT_H
#define __AIMBOT_H

#include <chrono>

#include <windows.h>

#include "SDK.hpp"
#include "config.h"
#include "util.h"

class Aimbot
{
    public:
    float m_LastAimDistance = 0.0f;

    void Run(Config& cfg, SDK::APlayerController* playerController)
    {
        if (playerController == nullptr || playerController->AcknowledgedPawn == nullptr)
        {
            return;
        }

        if (GetAsyncKeyState(AIMBOT_KEY) & 0x8000)
        {
            if (m_TargetPlayer == nullptr)
            {
                m_TargetPlayer = Util::GetClosestVisiblePlayer(playerController, cfg.m_MaxAimbot360LockDistance, !cfg.m_EnableAutofire);
                m_NextShotDeadline = m_Timer.now() + std::chrono::milliseconds(cfg.m_AutofireShotDelayMilliseconds);
            }
        }
        else
        {
            m_TargetPlayer = nullptr;
        }

        if (m_TargetPlayer != nullptr)
        {
            auto pawn = static_cast<SDK::AFortPawn*>(m_TargetPlayer);
            if (pawn->GetHealth() <= 0.0f || pawn->bIsDBNO)
            {
                m_TargetPlayer = nullptr;
                return;
            }

            SDK::FVector playerLoc;
            Util::Engine::GetBoneLocation(static_cast<SDK::ACharacter*>(m_TargetPlayer)->Mesh, &playerLoc, eBone::BONE_CHEST);

            auto dist = Util::GetDistance(playerController->AcknowledgedPawn->RootComponent->Location, playerLoc);
            m_LastAimDistance = dist;

            if (dist <= cfg.m_HeadshotMinDistance)
            {
                Util::Engine::GetBoneLocation(static_cast<SDK::ACharacter*>(m_TargetPlayer)->Mesh, &playerLoc, eBone::BONE_HEAD);
            }

            Util::LookAt(playerController, playerLoc);

            SDK::FVector zero{ 0.0f, 0.0f, 0.0f };
            auto lineOfSight = playerController->LineOfSightTo(m_TargetPlayer, zero, false);

            if (cfg.m_EnableAutofire)
            {
                if (!m_Firing && lineOfSight)
                {
                    if (m_NextShotDeadline <= m_Timer.now())
                    {
                        m_Firing = true;
                        LeftClick(true);
                    }
                }
                else if (m_Firing && !lineOfSight)
                {
                    m_Firing = false;
                    LeftClick(false);
                    m_NextShotDeadline = m_Timer.now() + std::chrono::milliseconds(cfg.m_AutofireShotDelayMilliseconds);
                }
                else if (m_Firing && (m_NextShotDeadline + std::chrono::milliseconds(cfg.m_AutofireShotDelayMilliseconds * 2) < m_Timer.now()))
                {
                    m_Firing = false;
                    LeftClick(false);
                    m_NextShotDeadline = m_Timer.now();
                }
            }
        }
        else if (m_Firing)
        {
            m_Firing = false;
            LeftClick(false);
            m_NextShotDeadline = m_Timer.now();
        }

    }

    private:
    bool m_Firing = false;
    SDK::AActor* m_TargetPlayer = nullptr;
    std::chrono::high_resolution_clock m_Timer;
    std::chrono::high_resolution_clock::time_point m_NextShotDeadline = m_Timer.now();

    void LeftClick(bool down)
    {
        INPUT Input = { 0 };
        ::ZeroMemory(&Input, sizeof(INPUT));
        Input.type = INPUT_MOUSE;

        if (down)
        {
            Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        }
        else
        {
            Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        }

        ::SendInput(1, &Input, sizeof(INPUT));
    }
};

#endif
