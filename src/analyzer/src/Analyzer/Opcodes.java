/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * http://www.gnu.org/copyleft/gpl.html
 */
 
//check emblem packet is 46 (on old setup)
	//
//id item is 16 (on old)
	//
	
//C_OPCODE_TRADE 83 to request trade (117)
//C_OPCODE_ATTR 112 to accept trade (45)
//C_OPCODE_TRADEADDITEM 28 for add item (
//send 30 when other person adds item
//C_OPCODE_TRADEADDCANCEL 25 to abort


//83
//28
//120

//trade, party server packets broken
	

//manage castle security 106
	//26
//declare siege time 101
	//62
//deposit adena to castle 129
	//43
//retrieve adena from castle 58
	//80
//mail is 57
	//
//party members 89
	//44


//learn spell, 51 to open list, 118 when you buy a spell

	
//private shop is 61 to open, 63 to buy something


//accuse = 30

package Analyzer;

public class Opcodes { 

	public Opcodes() {
	}

	public static final int C_OPCODE_TRADEADDOK = 0;	//click ok during trade
	public static final int C_OPCODE_TRADE = 1;			//begin trade
	public static final int C_OPCODE_SHOP = 2;
	public static final int C_OPCODE_DELBUDDY = 3;		//delete a buddy
	public static final int C_OPCODE_KEEPALIVE = 4;		//sent regularly by the client
	public static final int C_OPCODE_BANCLAN = 6;		//banish a person from the pledge
	public static final int C_OPCODE_DOOR = 8;			//open or close a door
	public static final int C_OPCODE_ARROWATTACK = 10;	//attack somebody with a bow
	public static final int C_OPCODE_RETURNTOLOGIN = 12;
	public static final int C_OPCODE_USESKILL = 13;		//use any skill or magic
	public static final int C_OPCODE_LOGINTOSERVER = 18;
	//accuse = 19
	public static final int C_OPCODE_BOARDBACK = 21;
	public static final int C_OPCODE_PARTY = 22;		//list party members?
	public static final int C_OPCODE_ADDBUDDY = 23;		//add a buddy
	public static final int C_OPCODE_BOARDREAD = 25;
	public static final int C_OPCODE_DELETECHAR = 26;	//delete a character
	//27 used for something unknown
	public static final int C_OPCODE_BOOKMARK = 29;		//create a bookmark
	public static final int C_OPCODE_QUITGAME = 30;
	public static final int C_OPCODE_SELECTLIST = 31;
	public static final int C_OPCODE_JOINCLAN = 33;		//join a pledge
	public static final int C_OPCODE_TITLE = 34;		//give somebody a title
	public static final int C_OPCODE_CHAT = 35;			//participate in chat
	public static final int C_OPCODE_DELETEINVENTORYITEM = 36;	//delete one or multiple items
	public static final int C_OPCODE_COMMONCLICK = 39;
	public static final int C_OPCODE_LOGINPACKET = 40;
	public static final int C_OPCODE_BOARDDELETE = 41;	//delete a message from a board
	//43 sent after 83 after character select
	public static final int C_OPCODE_TRADEADDCANCEL = 44;	//click cancel while trading
	//45 is manage castle inner security
	public static final int C_OPCODE_ATTACK = 47;		//attack somebody
	public static final int C_OPCODE_ATTR = 48;			//assign your lvl 50+ bonus point
	public static final int C_OPCODE_DRAWAL = 50;		//draw funds from the castle
	public static final int C_OPCODE_SELECTTARGET = 52;	//tell your pet to attack something
	public static final int C_OPCODE_CHANGEHEADING = 53;	//face a different direction
	public static final int C_OPCODE_EXCLUDE = 54;		// /exclude
	public static final int C_OPCODE_RESULT = 55;
	public static final int C_OPCODE_BASE_STAT = 56;
	public static final int C_OPCODE_NPCACTION = 58;	//click on an action in an npc chat
	public static final int C_OPCODE_CHATWHISPER = 59;	//whisper to somebody
	public static final int C_OPCODE_LEAVEPARTY = 60;	//leave a party
	public static final int C_OPCODE_BOARDWRITE = 62;	//write a message on a board
	public static final int C_OPCODE_BOARD = 66;
	public static final int C_OPCODE_BUDDYLIST = 67;	//view your buddy list
	public static final int C_OPCODE_EMBLEM = 68;		//upload your pledge emblem
	public static final int C_OPCODE_PETMENU = 69;		//bring up the pet menu?
	public static final int C_OPCODE_FISHCLICK = 70;	//go fishing, click on the lure
	public static final int C_OPCODE_CHATGLOBAL = 71;	//use global chat
	public static final int C_OPCODE_NEWCHAR = 72;		//create a new character
	public static final int C_OPCODE_FIGHT = 73;		//duel somebody
	public static final int C_OPCODE_FIX_WEAPON_LIST = 76;	//go try to fix your weapon
	public static final int C_OPCODE_USEPETITEM = 79;	//double-click a pets inventory item
	public static final int C_OPCODE_GIVEITEM = 82;		//give an item to somebody
	//83 sent when logging in, unsure (initGame) [not implemented]
	public static final int C_OPCODE_SKILLBUY = 84;		//bring up the spell buying list
	public static final int C_OPCODE_CLIENTVERSION = 85;	//first packet sent from client
	public static final int C_OPCODE_LOGINTOSERVEROK = 86;
	public static final int C_OPCODE_CHANGEWARTIME = 87;
	public static final int C_OPCODE_CREATECLAN = 88;		//create a pledge
	public static final int C_OPCODE_PLEDGE = 89;			//list pledge members
	public static final int C_OPCODE_TRADEADDITEM = 90;		//add item to trade
	public static final int C_OPCODE_CHANGECHAR = 93;		//when you click restart, but you are not dead
	public static final int C_OPCODE_AMOUNT = 94;		//amount of a craftable item to make
	public static final int C_OPCODE_CHARACTERCONFIG = 98;//98? (when you select quit) (and change characters)
	public static final int C_OPCODE_PICKUPITEM = 99;	//pick up an item from the ground
	public static final int C_OPCODE_USEITEM = 100;		//use any item
	public static final int C_OPCODE_PRIVATESHOPLIST = 101;	//
	public static final int C_OPCODE_BOOKMARKDELETE = 102;	//delete a bookmark
	public static final int C_OPCODE_CHECKPK = 103;		// /checkpk
	public static final int C_OPCODE_LEAVECLANE = 107;	//leave pledge
	public static final int C_OPCODE_SHIP = 108;		//sent at the end of a ship ride
	public static final int C_OPCODE_NPCTALK = 109;		//talk to any npc
	public static final int C_OPCODE_TAXRATE = 110;		//set the castle tax rate
	public static final int C_OPCODE_WAR = 115;			//declare war with another pledge
	public static final int C_OPCODE_MOVECHAR = 116;	//walk somewhere
	public static final int C_OPCODE_MAIL = 117;		//any mail functions
	public static final int C_OPCODE_WHO = 118;			// /who
	public static final int C_OPCODE_SKILLBUYOK = 119;	//actually buy a spell
	public static final int C_OPCODE_PROPOSE = 120;		// /propose (can be done anywhere)
	public static final int C_OPCODE_RESTART = 121;		//click restart when you are dead
	public static final int C_OPCODE_BANPARTY = 122;	//ban somebody from a party
	//alliance functions 123
	public static final int C_OPCODE_DEPOSIT = 124;		//put money in the castle treasury
	public static final int C_OPCODE_EXTCOMMAND = 125;	//when you press alt+1,2,3,4 while not in poly
	public static final int C_OPCODE_DROPITEM = 126;	//drop an item on the ground
	public static final int C_OPCODE_CREATEPARTY = 130;	//invite the first person to a party
	
//old clientpackets
	public static final int C_OPCODE_HIRESOLDIER = 565;
		//doesn't do anything
	public static final int C_OPCODE_CAHTPARTY = 138;
		//talking to a chat party (not a hunt party as with #)
	public static final int C_OPCODE_ENTERPORTAL = 161;
		//apparently used to enter certain dungeons
	
		//used to buy pets?
		
//109 is set siege time
//client packet 121 not used?
	
	public static final int C_OPCODE_RANK = 811;
	
	public static final int C_OPCODE_EXIT_GHOST = 850;
	
	public static final int C_OPCODE_CALL = 873;	//gm call player, not sure how to get this packet
////////////////////////////////////////////////////////////////////////
	public static final int C_OPCODE_TELEPORT = 885;
		//not used
	public static final int C_OPCODE_COMMONCLICK2 = 919; // new addition
	
	public static final int C_OPCODE_DELEXCLUDE = 223; // new addition




//packets that the client has sent
//121, send_save function
//30 /accuse
//13 ???

	public static final int S_OPCODE_BOOKMARKS = 0;
	public static final int S_OPCODE_ITEMSTATUS = 1;
	// no opcode for 2, just an extra logout / check login packet
	public static final int S_OPCODE_CHARVISUALUPDATE = 3;
	public static final int S_OPCODE_REMOVE_OBJECT = 4;
	public static final int S_OPCODE_CHANGEHEADING = 5;
	public static final int S_OPCODE_RESURRECTION = 6;
	public static final int S_OPCODE_DRAWAL = 7;
	public static final int S_OPCODE_EMBLEM = 8;
	public static final int S_OPCODE_LIGHT = 9;
	// no opcode for 10, just an extra logout / check login packet
	public static final int S_OPCODE_TRADE = 11;
	public static final int S_OPCODE_DEXUP = 12;
	public static final int S_OPCODE_BOARDREAD = 13;
	public static final int S_OPCODE_INVLIST = 14;
	// no opcode for 15, runs "portalPacket" function unknown usage by client
	public static final int S_OPCODE_ADDSKILL = 16;
	public static final int S_OPCODE_NEWMASTER = 16;
	public static final int S_OPCODE_SYSMSG = 17;
	public static final int S_OPCODE_GLOBALCHAT = 17; //intentionally sharing number
	public static final int S_OPCODE_RANGESKILLS = 18;
	public static final int S_OPCODE_UNKNOWN1 = 19;
	public static final int S_OPCODE_LOGINRESULT = 19;
	public static final int S_OPCODE_DETELECHAROK = 19;
	public static final int S_OPCODE_PACKETBOX = 20;
	public static final int S_OPCODE_ACTIVESPELLS = 20; //intentionally sharing number
	public static final int S_OPCODE_SKILLICONGFX = 20; //intentionally sharing number
	public static final int S_OPCODE_HOUSEMAP = 21;
	public static final int S_OPCODE_CHARDELETEOK = 21;
	public static final int S_OPCODE_PARALYSIS = 22;
	public static final int S_OPCODE_MPUPDATE = 23;
	public static final int S_OPCODE_ATTACKPACKET = 24;
	// no opcode for 25, runs "blinkPacket" function
	public static final int S_OPCODE_CHARTITLE = 26;
	public static final int S_OPCODE_OWNCHARATTRDEF = 27;
	// no opcode for 28, runs "attackAllPacket" function
	// no opcode for 29, runs quit and "noticePacket" function
	public static final int S_OPCODE_CHARLIST = 30;
	public static final int S_OPCODE_MAPID = 31;
	public static final int S_OPCODE_UNDERWATER = 31;
	public static final int S_OPCODE_INVIS = 32;
	public static final int S_OPCODE_NEWCHARWRONG = 33;
	public static final int S_OPCODE_BLUEMESSAGE2 = 34;
	public static final int S_OPCODE_HPMETER = 34;
	public static final int S_OPCODE_STRUP = 35;
//	public static final int S_OPCODE_UNKNOWN1 = 36;
	public static final int S_OPCODE_SHOWSHOPBUYLIST = 37;
	public static final int S_OPCODE_PINGTIME = 38;
	public static final int S_OPCODE_POISON = 39;
	public static final int S_OPCODE_SHOWHTML = 40;
	public static final int S_OPCODE_TRADEADDITEM = 41;
	public static final int S_OPCODE_TAXRATE = 42;
	public static final int S_OPCODE_RESTART = 43;
	public static final int S_OPCODE_DEPOSIT = 44;
	public static final int S_OPCODE_HOUSELIST = 45;
	public static final int S_OPCODE_SPMR = 46;
	// no opcode for 47, runs "mercenaryArrangePacket"
	public static final int S_OPCODE_CASTLEMASTER = 48;
	public static final int S_OPCODE_WHISPERCHAT = 49;
	// No opcode for 50, runs quit and "keyPacket" function
	public static final int S_OPCODE_YES_NO = 51;
	public static final int S_OPCODE_TRADESTATUS = 52;
	public static final int S_OPCODE_CHARLOCK = 52; //intentionally sharing number
	public static final int S_OPCODE_HPUPDATE = 53;
	public static final int S_OPCODE_EXP = 54;
	public static final int S_OPCODE_MATERIAL = 54;
	public static final int S_OPCODE_ATTRIBUTE = 55;
	public static final int S_OPCODE_ITEMNAME = 56;
	public static final int S_OPCODE_ABILITY = 57;
	public static final int S_OPCODE_SKILLHASTE = 58;
	public static final int S_OPCODE_WAR = 59;
	public static final int S_OPCODE_SELECTTARGET = 60;
	public static final int S_OPCODE_NORMALCHAT = 61;
	public static final int S_OPCODE_CHARPACK = 62;
	public static final int S_OPCODE_DROPITEM = 62;
	// no opcode for 63, runs "masterPacket" function
	public static final int S_OPCODE_GAMETIME = 64;
	public static final int S_OPCODE_NEWCHARPACK = 65;
	// no opcode for 66, runs "clonePacket" function
	public static final int S_OPCODE_LAWFUL = 67;
	public static final int S_OPCODE_VOICECHAT = 68;
	public static final int S_OPCODE_UNKNOWN2 = 68; //intentionally sharing number
	// no opcode for 69, runs "exchangeableSpellListPacket" function
	public static final int S_OPCODE_CHARAMOUNT = 70;
//	public static final int S_OPCODE_TELEPORT = 71;
	public static final int S_OPCODE_SERVERSTAT = 72;
	public static final int S_OPCODE_WEATHER = 72;
	public static final int S_OPCODE_CURSEBLIND = 73;
	public static final int S_OPCODE_USEMAP = 74;
	public static final int S_OPCODE_DELSKILL = 75;
	public static final int S_OPCODE_MAIL = 76;
	public static final int S_OPCODE_LETTER = 77;
	public static final int S_OPCODE_PRIVATESHOPLIST = 78;
	public static final int S_OPCODE_TRUETARGET = 79;
	public static final int S_OPCODE_BOARD = 80;
	public static final int S_OPCODE_EFFECTLOCATION = 81;
	// no opcode for 82, runs "serverListPacket" function
	public static final int S_OPCODE_SOUND = 83;
//	public static final int S_OPCODE_CHARPACK = 84;
	public static final int S_OPCODE_PUTSOLDIER = 85;
	public static final int S_OPCODE_ITEMAMOUNT = 86;
	public static final int S_OPCODE_MOVEOBJECT = 87;
	public static final int S_OPCODE_SHOWRETRIEVELIST = 88;
	// no opcode for 89, runs "archerArrangePacket" function
	// no opcode for 90, runs "mercenarySelectPacket" function
	public static final int S_OPCODE_SHOWSHOPSELLLIST = 91;
	public static final int S_OPCODE_SKILLSOUNDGFX = 92;
	public static final int S_OPCODE_CHANGENAME = 93;
	public static final int S_OPCODE_DOACTIONGFX = 94;
	public static final int S_OPCODE_SKILLICONSHIELD = 95;
//	public static final int S_OPCODE_NEWCHARWRONG = 96;
//	public static final int S_OPCODE_DETELECHAROK = 97;
	public static final int S_OPCODE_HIRESOLDIER = 98;
	public static final int S_OPCODE_SKILLBUY = 99;
	// no opcode for 100, runs "notEnoughForSpellPacket" function
	public static final int S_OPCODE_ITEMCOLOR = 101;
	// no opcode for 102, runs "pledgePacket" function
	// no opcode for 103, runs "changeItemUsePacket" function
	public static final int S_OPCODE_OWNCHARSTATUS = 104;
	public static final int S_OPCODE_IDENTIFYDESC = 105;
	public static final int S_OPCODE_OWNCHARSTATUS2 = 106;
	public static final int S_OPCODE_BLUEMESSAGE = 107;
	public static final int S_OPCODE_WARTIME = 108;
	public static final int S_OPCODE_LIQUOR = 109;
	public static final int S_OPCODE_SELECTLIST = 110;
	public static final int S_OPCODE_BLESSOFEVA = 111;
	public static final int S_OPCODE_PINKNAME = 112;
	public static final int S_OPCODE_POLY = 113;
	public static final int S_OPCODE_NPCSHOUT = 114;
	public static final int S_OPCODE_TELEPORT = 115;
	public static final int S_OPCODE_SKILLBRAVE = 116;
	// no opcode for 117, runs "wantedLoginPacket" function
	public static final int S_OPCODE_COMMONNEWS = 118;
	public static final int S_OPCODE_SERVERVERSION = 119;
	public static final int S_OPCODE_DELETEINVENTORYITEM = 120;
	public static final int S_OPCODE_ADDITEM = 121;
	// no opcode for 122, runs "bookListPacket" function
	public static final int S_OPCODE_DISCONNECT = 123;
	// no opcode for 124, runs quit function
	// no opcode for 125, runs quit or "rollResultPacket" function
	public static final int S_OPCODE_INPUTAMOUNT = 126;
	public static final int S_OPCODE_SERVERMSG = 127;

}