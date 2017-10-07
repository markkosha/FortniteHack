#ifndef __CONFIG_H
#define __CONFIG_H

#define AUTOFIRE_TOGGLE_KEY VK_XBUTTON1
#define AIMBOT_KEY VK_XBUTTON2
#define CHAMS_KEY VK_F8
#define ESP_KEY VK_F9

//

#define DEFAULT_FONT L"Verdana"
#define ENEMY_TEXT_COLOR Color{ 0.9f, 0.9f, 0.15f, 0.95f }

class Config
{
    public:
    bool m_EnableESP = true;
    bool m_EnableChams = true;
    bool m_EnableAutofire = true;
    float m_HeadshotMinDistance = 6000.0f;
    float m_MaxAimbot360LockDistance = 2000.0f;
    int m_AutofireShotDelayMilliseconds = 25;
};

#endif
