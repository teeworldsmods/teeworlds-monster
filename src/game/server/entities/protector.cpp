/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Copyright © 2013 Neox.                                                                                                */
/* If you are missing that file, acquire a complete release at https://www.teeworlds.com/forum/viewtopic.php?pid=106934  */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <engine/shared/config.h>
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "game/server/gamecontroller.h"
#include "protector.h"
#include "laser.h"

CProtector::CProtector(CGameWorld *pGameWorld, int Owner)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_PROJECTILE)
{
	m_Owner = Owner;
	m_Pos = GameServer()->GetPlayerChar(m_Owner)->m_Pos + vec2(0, -50);
	m_ReloadTimer = Server()->TickSpeed() * 2;

	GameWorld()->InsertEntity(this);
}

void CProtector::HitCharacter()
{
    if(m_ReloadTimer)
    {
        m_ReloadTimer --;
        return;
    }

    for(int i = 0; i < MAX_MONSTERS; i++)
    {
        CMonster *pHit = GameServer()->GetValidMonster(i);

        if(!pHit)
            continue;

        if(distance(m_Pos, pHit->GetPos()) > GameServer()->Tuning()->m_LaserReach)
            continue;

        if(!GameServer()->Collision()->IntersectLine(m_Pos, pHit->m_Pos, 0x0, 0x0))
        {
            vec2 Direction = normalize(pHit->m_Pos - m_Pos);

            new CLaser(GameWorld(), m_Pos, Direction, GameServer()->Tuning()->m_LaserReach, m_Owner);
            GameServer()->CreateSound(m_Pos, SOUND_RIFLE_FIRE);
            m_ReloadTimer = Server()->TickSpeed() * 2;
            return;
        }
    }
}

void CProtector::Reset()
{
	GameWorld()->DestroyEntity(this);
}

void CProtector::Tick()
{
    if(!GameServer()->GetPlayerChar(m_Owner))
    {
        Reset();
        return;
    }

    if(!GameServer()->m_apPlayers[m_Owner]->m_Upgrades.m_Protector)
    {
        Reset();
        return;
    }

    HitCharacter();

    if(!GameServer()->Collision()->CheckPoint(GameServer()->m_apPlayers[m_Owner]->GetCharacter()->m_Pos + vec2(0, -50)))
        m_Pos = GameServer()->m_apPlayers[m_Owner]->GetCharacter()->m_Pos + vec2(0, -50);
    else
        m_Pos = GameServer()->m_apPlayers[m_Owner]->GetCharacter()->m_Pos + vec2(0, 50);
}

void CProtector::Snap(int SnappingClient)
{
	if(NetworkClipped(SnappingClient))
		return;

	CNetObj_Laser *pObj = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_ID, sizeof(CNetObj_Laser)));
    if(!pObj)
        return;

    pObj->m_X = (int)m_Pos.x;
    pObj->m_Y = (int)m_Pos.y;
    pObj->m_FromX = (int)m_Pos.x;
    pObj->m_FromY = (int)m_Pos.y;
    pObj->m_StartTick = Server()->Tick();
}
