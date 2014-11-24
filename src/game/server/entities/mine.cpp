/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Copyright � 2013 Neox.                                                                                                */
/* If you are missing that file, acquire a complete release at https://www.teeworlds.com/forum/viewtopic.php?pid=106934  */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <engine/shared/config.h>
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "game/server/gamecontroller.h"
#include "mine.h"

CMine::CMine(CGameWorld *pGameWorld, vec2 Pos, int Owner)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_PROJECTILE)
{
	m_Pos = Pos;
	m_Owner = Owner;
	m_LifeTime = Server()->TickSpeed() * g_Config.m_SvMineLifetime;
	m_NeedToGive = true;

	GameWorld()->InsertEntity(this);
}

void CMine::HitCharacter()
{
	if(!GameWorld()->ClosestMonster(m_Pos, 20.f, 0x0))
        return;

    GameServer()->CreateExplosion(m_Pos, m_Owner, WEAPON_HAMMER, false);

    Reset();
}

void CMine::Reset()
{
    GameServer()->CreateExplosion(m_Pos, m_Owner, WEAPON_HAMMER, true);
    GameServer()->CreateSound(m_Pos, SOUND_GRENADE_EXPLODE);
    if(GameServer()->GetPlayerChar(m_Owner))
        GameServer()->GetPlayerChar(m_Owner)->m_MineNum = clamp(GameServer()->GetPlayerChar(m_Owner)->m_MineNum + 1, 0, g_Config.m_SvMaxMines);
	GameWorld()->DestroyEntity(this);
}

void CMine::Tick()
{
    if(!GameServer()->GetPlayerChar(m_Owner))
    {
        Reset();
        return;
    }

    if(!GameServer()->m_apPlayers[m_Owner]->m_Upgrades.m_Mine)
    {
        Reset();
        return;
    }

	if(m_LifeTime)
    {
        m_LifeTime --;
        HitCharacter();
        if(!m_LifeTime)
            Reset();
    }
}

void CMine::Snap(int SnappingClient)
{
	if(NetworkClipped(SnappingClient))
		return;

	CNetObj_Projectile *pObj = static_cast<CNetObj_Projectile *>(Server()->SnapNewItem(NETOBJTYPE_PROJECTILE, m_ID, sizeof(CNetObj_Projectile)));
	if(!pObj)
		return;

	pObj->m_X = (int)m_Pos.x;
	pObj->m_Y = (int)m_Pos.y;
	pObj->m_VelX = 1;
	pObj->m_VelY = 1;
	pObj->m_Type = WEAPON_RIFLE;
}
