require("udp");

local Cfg = {};
Cfg.IP = "127.0.0.1";
Cfg.Port = 1234;
Cfg.Password = "******";

util.AddNetworkString('GlobalChatMessage');
util.AddNetworkString('GlobalChatUser');

local function GetServerIP()
        local hostip = GetConVarString( "hostip" )
        hostip = tonumber( hostip )
       
        local ip = {}
        ip[ 1 ] = bit.rshift( bit.band( hostip, 0xFF000000 ), 24 )
        ip[ 2 ] = bit.rshift( bit.band( hostip, 0x00FF0000 ), 16 )
        ip[ 3 ] = bit.rshift( bit.band( hostip, 0x0000FF00 ), 8 )
        ip[ 4 ] = bit.band( hostip, 0x000000FF )
       
        return table.concat( ip, "." )
end

hook.Add( "PlayerSay", "GlobalChatPlayerSay",  function( ply, text, public )

		local Query = {};
		Query.Password = Cfg.Password;
		Query.Type = "chat";
		Query.Body = {};
		Query.Body.Group = ply:GetUserGroup();
		Query.Body.PlayerName = ply:Nick();
		Query.Body.SteamID = ply:SteamID();
		Query.Body.Message = text;

		local JsonString = util.TableToJSON( Query );
		
		
		UDPSend(Cfg.Port, Cfg.IP, JsonString, string.len(JsonString) );
		
		
end);




timer.Create("GlobalChatConnection", 0.1, 0, function()
	local data = UDPReceive(Cfg.Port, Cfg.IP);
	
	if data == nil then
		return;

	end

	local JSONIn = util.JSONToTable(data);
	if !JSONIn then
		return;
	end
	
	if JSONIn.Pong or JSONIn.Ping then
		return
	end

	if JSONIn.Type == "chat" then
		net.Start("GlobalChatMessage");
			net.WriteString(JSONIn.Body.PlayerName);

			if JSONIn.Body.Group != nil then -- We have different versions
				net.WriteBool( true );
				net.WriteString(JSONIn.Body.Group);
			else
				net.WriteBool( false );
			end
			net.WriteString(JSONIn.Body.Message);
		net.Broadcast();
	end

	if JSONIn.Type == "user" then
		net.Start("GlobalChatUser");
			net.WriteString(JSONIn.Body.IP); -- Write Server IP
			net.WriteInt(JSONIn.Body.PlayerCount, 16); -- Write Player Count
			for key, ply in pairs(JSONIn.Body.Players) do
				net.WriteString(JSONIn.Body.Players[key].Nick); -- Write Player[X] Nickname
				net.WriteString(JSONIn.Body.Players[key].SteamID); -- Write Player[X] Nickname
				net.WriteString(JSONIn.Body.Players[key].Group); -- Write Player[X] Group
			end
		net.Broadcast();
	end

end);

-- This is an important message. It keeps the server in the listeners list for the master server
timer.Create("GlobalChatPing", 10, 0, function()
	if #player.GetAll() > 0 then
		local Query = {};
		Query.Password = Cfg.Password;
		Query.Ping = true;

		-- Send the packet
		local packet = util.TableToJSON( Query );
		
		UDPSend(Cfg.Port, Cfg.IP, packet, string.len(packet) );

	end
end);

-- udp:Connect(Cfg.IP, Cfg.Port);


function GlobalChat_UpdatePlayerState( unused )
	SendChatState();
end

function SendChatState()
		local Query = {};
		Query.Password = Cfg.Password;
		Query.Type = "user";
		Query.Body = {};
		Query.Body.IP = GetServerIP() .. ":"..GetConVarString("hostport");
		Query.Body.Players = {};
		Query.Body.PlayerCount = #player.GetAll();
		
		for key, ply in pairs(player.GetAll()) do
			Query.Body.Players[key] = {};
			Query.Body.Players[key].Nick = ply:Nick();
			Query.Body.Players[key].SteamID = ply:SteamID();
			Query.Body.Players[key].Group = ply:GetUserGroup();
		end

		local JsonString = util.TableToJSON( Query );
		UDPSend(Cfg.Port, Cfg.IP, JsonString, string.len(JsonString) );

end

hook.Add( "PlayerInitialSpawn", "GlobalChat_PlayerInitialSpawn", GlobalChat_UpdatePlayerState );
hook.Add( "PlayerDisconnected", "GlobalChat_PlayerDisconnected", GlobalChat_UpdatePlayerState );
