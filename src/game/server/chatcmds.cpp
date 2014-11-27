/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Copyright � 2013 Neox.                                                                                                */
/* If you are missing that file, acquire a complete release at https://www.teeworlds.com/forum/viewtopic.php?pid=106934  */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <game/server/gamecontext.h>
#include <engine/shared/config.h>
#include "entities/protector.h"
#include <stdio.h>

void CGameContext::ExecuteCommand(int ClientID, const char* pCmd)
{
    CPlayer* pPlayer = m_apPlayers[ClientID];
    pCmd ++; // Skip the '/'

    if(!str_comp_nocase(pCmd, "cmdlist"))
    {
        SendChatTarget(ClientID, "~~~~CMDLIST~~~~");
        SendChatTarget(ClientID, "/info - Informations about the mod");
        SendChatTarget(ClientID, "/help - Help about the mod");
        SendChatTarget(ClientID, "/shop - Buy some upgrades");
        SendChatTarget(ClientID, "/sendpoints <player name> <amount> - send points to player");
    }
    else if(!str_comp_nocase(pCmd, "info"))
    {
        SendChatTarget(ClientID, "~~~~INFO~~~~");
        SendChatTarget(ClientID, "Mod made by Neox");
        SendChatTarget(ClientID, "You can download this mod from the official Teeworlds forum.");
    }
    else if(!str_comp_nocase(pCmd, "help"))
    {
        SendChatTarget(ClientID, "~~~~HELP~~~~");
        SendChatTarget(ClientID, "You need to kill monsters, get points, and with these points you can buy upgrades");
        SendChatTarget(ClientID, "These upgrades will help you fight against monsters easier");
        SendChatTarget(ClientID, "As example you can buy the regeneration, then you will get health or armor every second");
        SendChatTarget(ClientID, "Your kills are your points. So when you buy something, you lose points");
        SendChatTarget(ClientID, "If you wanna look at the shop, just type \"/shop\"");
        SendChatTarget(ClientID, "Have fun.");
    }
    else if(!str_comp_nocase(pCmd, "shop"))
    {
        SendChatTarget(ClientID, "~~~~SHOP~~~~");
        SendChatTarget(ClientID, "/GetNinja - 10 pts");
        SendChatTarget(ClientID, "/GetWeapons - 15 pts");
        SendChatTarget(ClientID, "/Health <amount> - 15 pts per health");
        SendChatTarget(ClientID, "/dmg <amount> - 50 pts per extra dmg");
        SendChatTarget(ClientID, "/Regeneration - 20 pts");
        SendChatTarget(ClientID, "/AutoWeapons - 20 pts");
        SendChatTarget(ClientID, "/Mine - 30 pts");
        SendChatTarget(ClientID, "/InfiniteAmmo - 30 pts");
        SendChatTarget(ClientID, "/Vampirism - 40 pts");
        SendChatTarget(ClientID, "/Spread - 50 pts");
        SendChatTarget(ClientID, "/ShootingSpeed - 50 pts");
        SendChatTarget(ClientID, "/Protector - 60 pts");
        SendChatTarget(ClientID, "/DmgReflect - 60 pts");
        SendChatTarget(ClientID, "/PowerfulWeapons - 100 pts");
    }
    else if(!str_comp_nocase(pCmd, "getninja"))
    {
        if(!GetPlayerChar(ClientID))
        {
            SendChatTarget(ClientID, "Wait to respawn to do this");
            return;
        }
        if(pPlayer->m_Score >= 10)
        {
            pPlayer->m_Score -= 10;
            GetPlayerChar(ClientID)->GiveNinja();
        }
        else
            SendChatTarget(ClientID, "You need at least 10 points to buy this");
    }
    else if(!str_comp_nocase(pCmd, "getweapons"))
    {
        if(!GetPlayerChar(ClientID))
        {
            SendChatTarget(ClientID, "Wait to respawn to do this");
            return;
        }
        if(pPlayer->m_Score >= 15)
        {
            pPlayer->m_Score -= 15;
            for(int i = 1; i < NUM_WEAPONS-1; i++)
                GetPlayerChar(ClientID)->GiveWeapon(i, 10);
            SendChatTarget(ClientID, "You now have every weapons");
        }
        else
            SendChatTarget(ClientID, "You need at least 15 points to buy this");
    }
    else if(!str_comp_nocase_num(pCmd, "health", 6))
    {
        int Amount;
        if(sscanf(pCmd, "health %d", &Amount) != 1)
        {
            SendChatTarget(ClientID, "Please stick to the given structure :");
            SendChatTarget(ClientID, "/health <amount>");
            return;
        }
        if(Amount < 1)
        {
            SendChatTarget(ClientID, "The minimum amount is 1");
            return;
        }
        if(Amount > 1000000)
        {
            SendChatTarget(ClientID, "The amount is too high");
            return;
        }
        char aBuf[128];
        if(pPlayer->m_Score >= Amount * 15)
        {
            pPlayer->m_Score -= Amount * 15;
            pPlayer->m_Upgrades.m_Health += Amount;
            str_format(aBuf, sizeof(aBuf), "You have bought %d health", Amount);
        }
        else
            str_format(aBuf, sizeof(aBuf), "If you want to buy %d health, you need at least %d points", Amount, Amount * 15);

        SendChatTarget(ClientID, aBuf);
    }
    else if(!str_comp_nocase_num(pCmd, "dmg ", 4))
    {
        int Amount;
        if(sscanf(pCmd, "dmg %d", &Amount) != 1)
        {
            SendChatTarget(ClientID, "Please stick to the given structure :");
            SendChatTarget(ClientID, "/dmg <amount>");
            return;
        }
        if(Amount < 1)
        {
            SendChatTarget(ClientID, "The minimum amount is 1");
            return;
        }
        if(Amount > 1000000)
        {
            SendChatTarget(ClientID, "The amount is too high");
            return;
        }
        char aBuf[128];
        if(pPlayer->m_Score >= Amount * 50)
        {
            pPlayer->m_Score -= Amount * 50;
            pPlayer->m_Upgrades.m_Dmg += Amount;
            str_format(aBuf, sizeof(aBuf), "You have bought %d extra damage", Amount);
        }
        else
            str_format(aBuf, sizeof(aBuf), "If you want to buy %d extra damage, you need at least %d points", Amount, Amount * 50);
        
        SendChatTarget(ClientID, aBuf);
    }
    else if(!str_comp_nocase(pCmd, "regeneration"))
    {
        if(pPlayer->m_Score >= 20 && !pPlayer->m_Upgrades.m_Regeneration)
        {
            pPlayer->m_Score -= 20;
            pPlayer->m_Upgrades.m_Regeneration = true;
            SendChatTarget(ClientID, "You now have the regeneration");
        }
        else if(pPlayer->m_Upgrades.m_Regeneration)
            SendChatTarget(ClientID, "You already have the regeneration");
        else
            SendChatTarget(ClientID, "You need at least 20 points to buy this");
    }
    else if(!str_comp_nocase(pCmd, "autoweapons"))
    {
        if(pPlayer->m_Score >= 20 && !pPlayer->m_Upgrades.m_AutoWeapons)
        {
            pPlayer->m_Score -= 20;
            pPlayer->m_Upgrades.m_AutoWeapons = true;
            SendChatTarget(ClientID, "You now have the automatic weapons");
        }
        else if(pPlayer->m_Upgrades.m_AutoWeapons)
            SendChatTarget(ClientID, "You already have the automatic weapons");
        else
            SendChatTarget(ClientID, "You need at least 20 points to buy this");
    }
    else if(!str_comp_nocase(pCmd, "mine"))
    {
        if(pPlayer->m_Score >= 30 && !pPlayer->m_Upgrades.m_Mine)
        {
            pPlayer->m_Score -= 30;
            pPlayer->m_Upgrades.m_Mine = true;
            SendChatTarget(ClientID, "You now have the mines");
        }
        else if(pPlayer->m_Upgrades.m_Mine)
            SendChatTarget(ClientID, "You already have the mines");
        else
            SendChatTarget(ClientID, "You need at least 30 points to buy this");
    }
    else if(!str_comp_nocase(pCmd, "infiniteammo"))
    {
        if(pPlayer->m_Score >= 30 && !pPlayer->m_Upgrades.m_InfAmmo)
        {
            pPlayer->m_Score -= 30;
            pPlayer->m_Upgrades.m_InfAmmo = true;
            SendChatTarget(ClientID, "You now have infinite ammo");
            if(GetPlayerChar(ClientID))
            {
                for(int i = 1; i < NUM_WEAPONS-1; i ++)
                {
                    if(GetPlayerChar(ClientID)->GotWeapon(i))
                        GetPlayerChar(ClientID)->SetAmmo(i, 10);
                }
            }
        }
        else if(pPlayer->m_Upgrades.m_InfAmmo)
            SendChatTarget(ClientID, "You already have infinite ammo");
        else
            SendChatTarget(ClientID, "You need at least 30 points to buy this");
    }
    else if(!str_comp_nocase(pCmd, "vampirism"))
    {
        if(pPlayer->m_Score >= 40 && !pPlayer->m_Upgrades.m_Vampirism)
        {
            pPlayer->m_Score -= 40;
            pPlayer->m_Upgrades.m_Vampirism = true;
            SendChatTarget(ClientID, "You now have the vampirism");
        }
        else if(pPlayer->m_Upgrades.m_Vampirism)
            SendChatTarget(ClientID, "You already have the vampirism");
        else
            SendChatTarget(ClientID, "You need at least 40 points to buy this");
    }
    else if(!str_comp_nocase(pCmd, "spread"))
    {
        if(pPlayer->m_Score >= 50 && !pPlayer->m_Upgrades.m_Spread)
        {
            pPlayer->m_Score -= 50;
            pPlayer->m_Upgrades.m_Spread = true;
            SendChatTarget(ClientID, "You now have the spreading weapons");
        }
        else if(pPlayer->m_Upgrades.m_Spread)
            SendChatTarget(ClientID, "You already have the spreading weapons");
        else
            SendChatTarget(ClientID, "You need at least 50 points to buy this");
    }
    else if(!str_comp_nocase(pCmd, "shootingspeed"))
    {
        if(pPlayer->m_Score >= 50 && !pPlayer->m_Upgrades.m_ShootingSpeed)
        {
            pPlayer->m_Score -= 50;
            pPlayer->m_Upgrades.m_ShootingSpeed = true;
            SendChatTarget(ClientID, "You now have the shooting speed");
        }
        else if(pPlayer->m_Upgrades.m_ShootingSpeed)
            SendChatTarget(ClientID, "You already have the shooting speed");
        else
            SendChatTarget(ClientID, "You need at least 50 points to buy this");
    }
    else if(!str_comp_nocase(pCmd, "protector"))
    {
        if(pPlayer->m_Score >= 60 && !pPlayer->m_Upgrades.m_Protector)
        {
            pPlayer->m_Score -= 60;
            pPlayer->m_Upgrades.m_Protector = true;
            SendChatTarget(ClientID, "You now have the protector");
            if(GetPlayerChar(ClientID))
                new CProtector(&m_World, ClientID);
        }
        else if(pPlayer->m_Upgrades.m_Protector)
            SendChatTarget(ClientID, "You already have the protector");
        else
            SendChatTarget(ClientID, "You need at least 60 points to buy this");
    }
    else if(!str_comp_nocase(pCmd, "dmgreflect"))
    {
        if(pPlayer->m_Score >= 60 && !pPlayer->m_Upgrades.m_DmgReflect)
        {
            pPlayer->m_Score -= 60;
            pPlayer->m_Upgrades.m_DmgReflect = true;
            SendChatTarget(ClientID, "You now have the damage reflector");
        }
        else if(pPlayer->m_Upgrades.m_DmgReflect)
            SendChatTarget(ClientID, "You already have the damage reflector");
        else
            SendChatTarget(ClientID, "You need at least 60 points to buy this");
    }
    else if(!str_comp_nocase(pCmd, "powerfulweapons"))
    {
        if(pPlayer->m_Score >= 100 && !pPlayer->m_Upgrades.m_PowerfulWeapons && pPlayer->GotAll())
        {
            pPlayer->m_Score -= 100;
            pPlayer->m_Upgrades.m_PowerfulWeapons = true;
            SendChatTarget(ClientID, "You now have the powerful weapons");
        }
        else if(pPlayer->m_Upgrades.m_PowerfulWeapons)
            SendChatTarget(ClientID, "You already have the powerful weapons");
        else if(!pPlayer->GotAll())
            SendChatTarget(ClientID, "You need to have all the shop to buy this");
        else
            SendChatTarget(ClientID, "You need at least 100 points to buy this");
    }
    else if(!str_comp_nocase_num(pCmd, "sendpoints", 10))
    {
        int Amount;
        char Amountchar[64];
        char pCmdBuf[str_length(pCmd) + 1];
        mem_copy(pCmdBuf, pCmd, str_length(pCmd) + 1);
        
        if(str_length(pCmd) > 64)
        {
            SendChatTarget(ClientID, "your message is too long");
            return;
        }
        
        for (int i = str_length(pCmd) - 1; true; i--) // cut off the number from the chatmessage
        {
        	pCmdBuf[i] = '\0'; // cut it off
            if(!(pCmd[i] >= '0' && pCmd[i] <= '9'))
            {
                if(pCmd[i] == ' ')
                {
                    mem_copy(Amountchar, &pCmd[i+1], str_length(pCmd) - i); // save it in Amountchar
                    Amount = str_toint(Amountchar); // make int out of it
                    break;
                }
                else
                {
                    SendChatTarget(ClientID, "Please stick to the given structure :");
                    SendChatTarget(ClientID, "/sendpoints <player name> <amount>");
                    return;
                }
            }
        }
        
        if(str_length(pCmdBuf) <= 11) // check the player name
        {
        	SendChatTarget(ClientID, "Please stick to the given structure :");
        	SendChatTarget(ClientID, "/sendpoints <player name> <amount>");
        	return;
        }
        
        char name[str_length(pCmdBuf) - 11 + 1];
        mem_move(name, pCmdBuf + 11, str_length(pCmdBuf) - 11 + 1);

        if(Amount < 1)
        {
            SendChatTarget(ClientID, "The minimum amount is 1");
            return;
        }
        if(Amount > pPlayer->m_Score)
        {
            SendChatTarget(ClientID, "You don't have that much money");
            return;
        }
        for(int i = 0; i <= 16; i++)
        {
            if(Server()->ClientIngame(i) && !str_comp(name, (Server()->ClientName(i))))
            {
                pPlayer->m_Score -= Amount;
                m_apPlayers[i]->m_Score += Amount;
                char aBuf[256];
                str_format(aBuf, sizeof(aBuf), "You have succesfully sent %d point(s) to %s", Amount, name); // Done \o/
                SendChatTarget(ClientID, aBuf);
                
                char aBuf2[256];
                str_format(aBuf2, sizeof(aBuf2), "%s sent you %d point(s)", Server()->ClientName(ClientID), Amount);
                SendChatTarget(i, aBuf2);
                break;
            }
            if(i >= 16)
            {
            	char aBuf3[256];
            	str_format(aBuf3, sizeof(aBuf3), "This player does not exist (\"%s\")", name);
                SendChatTarget(ClientID, aBuf3);
            }
        }
    }
    else if(!str_comp_nocase_num(pCmd, "setwave", 7))
    {
        int number;
        if(sscanf(pCmd, "setwave %d", &number) != 1)
        {
            SendChatTarget(ClientID, "Please stick to the given structure :");
            SendChatTarget(ClientID, "/setwave <number>");
            return;
        }
        if(number < 1)
        {
            SendChatTarget(ClientID, "The minimum number is 1");
            return;
        }
        if(number > 1000000)
        {
            SendChatTarget(ClientID, "The number is too high");
            return;
        }
        
        if(m_VoteCloseTime)
        {
             SendChatTarget(ClientID, "Wait for current vote to end before calling a new one.");
             return;
        }

             int Timeleft = pPlayer->m_LastVoteCall + Server()->TickSpeed()*60 - Server()->Tick();
             if(pPlayer->m_LastVoteCall && Timeleft > 0)
             {
                   char aChatmsg[512] = {0};
                   str_format(aChatmsg, sizeof(aChatmsg), "You must wait %d seconds before making another vote", (Timeleft/Server()->TickSpeed())+1);
                   SendChatTarget(ClientID, aChatmsg);
                   return;
             }
             else
             {
            	 char aBuf[32];
            	 char aBuf2[32];
            	 str_format(aBuf, sizeof(aBuf), "set_wave %d", number);
            	 str_format(aBuf2, sizeof(aBuf2), "%d", number);
            	 
            	 char aChatmsg [512] = {0};
            	 str_format(aChatmsg, sizeof(aChatmsg), "'%s' called vote to change server option '%s %s'", Server()->ClientName(ClientID), "set wave to:", aBuf2);
            	 SendChat(-1, CGameContext::CHAT_ALL, aChatmsg);
            	 
            	 StartVote("set wave to", aBuf, aBuf2);
            	 pPlayer->m_Vote = 1;
                 pPlayer->m_VotePos = m_VotePos = 1;
                 m_VoteCreator = ClientID;
                 pPlayer->m_LastVoteCall = Server()->Tick();
             }
    }
    else
    {
        char aBuf[512];
        str_format(aBuf, sizeof(aBuf), "Typed : /%s", pCmd);
        SendChatTarget(ClientID, "This command doesn't exist, try \"/cmdlist\".");
        SendChatTarget(ClientID, aBuf);
    }
}
