package Analyzer;

import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.lang.InterruptedException;
import java.lang.Thread;
import java.net.Socket;
import java.util.Random;

//import jpcap.*;
//import jpcap.packet.Packet;
//import jpcap.packet.TCPPacket;

import Config.Config;
import encryptions.ClientIdExistsException;
import encryptions.LineageEncryption;
import encryptions.LineageKeys;
import types.UByte8;
import types.UChar8;


//log4j-*.*.**.jar for these imports
import org.apache.log4j.Logger;

//this class will have strange results if it does not pick up on the first packet
class ConversationUpdate implements Runnable
{	//follows conversations, one of these per client/server connection
	private static Logger _log = Logger.getLogger(ConversationUpdate.class.getName());
	
	//these are used for direct communication with the mentioned element
	private Socket _client_me;
	private Socket _server_me;
	
	private InputStream _from_client;
	private InputStream _from_server;
	private OutputStream _to_client;
	private OutputStream _to_server;

/*	public void receivePacket(Packet packet) 
	{
		TCPPacket temp;
		if (packet instanceof TCPPacket)
		{
			temp = (TCPPacket) packet;
			if ((temp.dst_port == Config.PORT_NUM) || (temp.src_port == Config.PORT_NUM))
			{
				int length = temp.data[0] + (temp.data[1] * 0x100);
				
				if (_server.compareTo(temp.src_ip.getHostAddress()) == 0)
				{	//server to client packet
					_log.info("Received packet from server");
					try
					{
						_server_to_client.addData(temp.data);
						if (_sc_length == 0)
						{	
							if (_server_to_client.available() >= 2)
							{	//read packet length if there is both bytes are present
								int hiByte = _server_to_client.read();
								int loByte = _server_to_client.read();
								_sc_length = (loByte * 0x100 + hiByte) - 2;
//								_log.info("Length: " + length + ", " + _sc_length + ", " + _pr);
							}
						}
						if ((_sc_length > 0) && (_server_to_client.available() >= _sc_length))
						{	//read a packet
							readPacket(_server_to_client, _sc_length, 0);
							_sc_length = 0;
						}
					}
					catch (Exception e)
					{
						_log.fatal(e.getLocalizedMessage());
					}
				}
				else
				{
					_log.info("Received packet from client");
					try
					{
						_client_to_server.addData(temp.data);
						if (_cs_length == 0)
						{
							if (_client_to_server.available() >= 2)
							{	//read packet length if there is both bytes are present
								int hiByte = _client_to_server.read();
								int loByte = _client_to_server.read();
							
								_cs_length = (loByte * 0x100 + hiByte) - 2;
//								_log.info("Length: " + length + ", " + _cs_length + ", " + _pr);
							}
						}
						if ((_cs_length > 0) && (_client_to_server.available() >= _cs_length))
						{	//read a packet
							readPacket(_client_to_server, _cs_length, 1);
							_cs_length = 0;
						}
					}
					catch (Exception e)
					{
						_log.fatal(e.getLocalizedMessage());
					}
				}
								
				_pr = _pr + 1;
			}
		}
	}*/

	public ConversationUpdate(Socket from_client) throws IOException 
	{	//this is passed a socket to be used with the client
		//might possibly need 3 threads
			//one for client to server passing
			//another for server to client passing
			//and a third for doing other stuff
			
	
		_client_me = from_client;	//save the socket for communication with the client	
		//create streams to and from the client
		_from_client = _client_me.getInputStream();
		_to_client = _client_me.getOutputStream();
	
		//open a connection to the server		
		_server_me = new Socket(Config.SERVER, 2003);
		
		//create streams to and from server
		_from_server = _server_me.getInputStream();
		_to_server = _server_me.getOutputStream();
	}
	
	public void run()
	{
		try 
		{
			byte [] buffer = new byte[4];
			_from_client.read(buffer, 0, 4);
			_log.info("Read: " + buffer[0] + ", " + buffer[1] + ", " + buffer[2] + ", " + buffer[3]);
			_to_server.write(buffer);
			
			int done_yet = 0;
			while (done_yet == 0)
			{
				int temp = _from_server.read();
				if (temp != -1)
				{
					_to_client.write(temp);
				}
				else
				{
					//disconnect from client
					_client_me.close();
					done_yet = 1;
				}
			}
		}  
		catch (Exception e) 
		{
			 _log.fatal(e.getLocalizedMessage());
		}
	}
}

