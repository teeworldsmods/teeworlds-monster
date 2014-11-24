/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Copyright � 2013 Neox.                                                                                                */
/* If you are missing that file, acquire a complete release at https://www.teeworlds.com/forum/viewtopic.php?pid=106934  */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <engine/shared/config.h>
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "game/server/gamecontroller.h"
#include "wall.h"

CWall::CWall(CGameWorld *pGameWorld, vec2 From, vec2 To, int Owner)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_LASER)
{
	m_Pos = To;
	m_From = From;
	m_Owner = Owner;
	m_ID2 = Server()->SnapNewID();
	m_Kills = 0;
	GameWorld()->InsertEntity(this);
}

void CWall::HitCharacter()
{
    for(int i = 0; i < MAX_MONSTERS; i ++)
    {
        CMonster* pHit = GameServer()->GetValidMonster(i);

        if(!pHit)
            continue;

        if(m_aReloadTimers[i])
            continue;

        vec2 IntersectPos = closest_point_on_line(m_Pos, m_From, pHit->GetPos());
		float Len = distance(pHit->GetPos(), IntersectPos);
		if(Len < pHit->m_ProximityRadius+2.0f)
        {
            m_aReloadTimers[i] = Server()->TickSpeed();
            if(g_Config.m_SvWallDamage > 0)
                pHit->TakeDamage(vec2(0, 0), g_Config.m_SvWallDamage, m_Owner, WEAPON_HAMMER, false, false);
            else if (g_Config.m_SvWallDamage == -1)
                pHit->TakeDamage(vec2(0, 0), pHit->GetLifes(), m_Owner, WEAPON_HAMMER, false, false);
                
                
            
            if(!GameServer()->GetValidMonster(i)) // If the monster is dead
            {
                m_Kills ++;
                if(m_Kills >= g_Config.m_SvMaxWallKills)
                {
                    Reset();
                    return;
                }
            }
        }
    }
}

void CWall::Reset()
{
    if(GameServer()->GetPlayerChar(m_Owner))
        GameServer()->GetPlayerChar(m_Owner)->m_WallNum = clamp(GameServer()->GetPlayerChar(m_Owner)->m_WallNum + 1, 0, g_Config.m_SvMaxWalls);

	GameServer()->m_World.DestroyEntity(this);

    Server()->SnapFreeID(m_ID2);
}
void CWall::Tick()
{
    if(!GameServer()->GetPlayerChar(m_Owner) || GameServer()->m_pController->m_PreparTimer)
    {
        Reset();
        return;
    }

    for(int i = 0; i < MAX_MONSTERS; i ++)
        if(m_aReloadTimers[i])
            m_aReloadTimers[i] --;

    HitCharacter();
}
void CWall::Snap(int SnappingClient)
{
	CNetObj_Laser *pObj = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_ID, sizeof(CNetObj_Laser)));
	if(!pObj)
		return;
	pObj->m_X = (int)m_Pos.x;
	pObj->m_Y = (int)m_Pos.y;
	pObj->m_FromX = (int)m_From.x;
	pObj->m_FromY = (int)m_From.y;
	pObj->m_StartTick = Server()->Tick()-3;

	CNetObj_Laser *pObj2 = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_ID2, sizeof(CNetObj_Laser)));
	if(!pObj2)
		return;
	pObj2->m_X = (int)m_From.x;
	pObj2->m_Y = (int)m_From.y;
	pObj2->m_FromX = (int)m_From.x;
	pObj2->m_FromY = (int)m_From.y;
    pObj2->m_StartTick = Server()->Tick()-3;
}
