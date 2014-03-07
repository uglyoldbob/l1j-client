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

class ServerClient implements Runnable
{	//used to receive messages and pass them to the client
		//along with additional messages
	private static Logger _log = Logger.getLogger(ServerClient.class.getName());
	
	private Conversation _parent;	//used to report data to the parent
	
	private InputStream _from_server;
	private OutputStream _to_client;
		
	private LineageKeys _clkey;
	private int _key_initialized;

	private long _seed;
	private int _done;

	public ServerClient(InputStream from, OutputStream to, Conversation parent) throws IOException 
	{	
		_from_server = from;
		_to_client = to;
		_parent = parent;
		_key_initialized = 0;	//we will not forward any packets until we get the encryption key
		_done = 0;
	}
	
	public void sendPacket(byte[] abyte0, int use_key) 
	{
		synchronized (this) 
		{
			try 
			{
				//byte abyte0[] = packet.getContent();
//				_log.info("Send " + abyte0[0] + " to " + getAccountName() + ":" + _hostname);
				char ac[] = new char[abyte0.length];
				ac = UChar8.fromArray(abyte0);
				if ((_key_initialized == 1) && (use_key == 1))
					ac = LineageEncryption.encrypt(ac, _clkey);
				abyte0 = UByte8.fromArray(ac);
				int j = abyte0.length + 2;

				_to_client.write(j & 0xff);
				_to_client.write(j >> 8 & 0xff);
				
				_to_client.write(abyte0);
				_to_client.flush();
			}
			catch (Exception e) 
			{
				_log.warn(e.getLocalizedMessage());
			}
		}
	}
	
	public void initEncryption(long seed)
	{	//called when the seed is extracted from the first packet (sent from the server)
		try 
		{
			_clkey = LineageEncryption.initKeys(this, seed);
			//send the encryption key to the other direction handler
			_parent._cs.initEncryption(seed);
			_key_initialized = 1;
		}
		catch (ClientIdExistsException e)
		{
			_log.fatal("ClientIdExistsException");
		}
		
	}
	
	private void printBuffer(byte[] printme)
	{	//logs the contents of a buffer
		_log.info(toHexString(printme));
	
		for (int counter = 0; counter < printme.length; counter += 16)
		{
			String row_output = " ";
			int row_amount = printme.length - counter;
			for (int tally = 0; tally < row_amount; tally++)
			{
				int sampling = printme[counter+tally] & 0xFF;
				row_output = row_output + sampling + " ";
			}
			_log.info(row_output);
		}
	}

	private char[] hexChar = {
	   '0' , '1' , '2' , '3' ,
	   '4' , '5' , '6' , '7' ,
	   '8' , '9' , 'a' , 'b' ,
	   'c' , 'd' , 'e' , 'f'};
			
	private String toHexString ( byte[] b )
	{
		StringBuffer sb = new StringBuffer( b.length * 2 );
		for ( int i=0; i<b.length; i++ )
			{
			// look up high nibble char
			sb.append( hexChar [( b[i] & 0xf0 ) >>> 4] );
	
			// look up low nibble char
			sb.append( hexChar [b[i] & 0x0f] );
			sb.append(' ');
			}
		return sb.toString();
	}
	
	private byte[] readPacket() throws Exception 
	{
		try 
		{
			int hiByte = _from_server.read();
			int loByte = _from_server.read();
			if (loByte < 0) 
			{
				throw new RuntimeException();
			}
			int dataLength = (loByte * 256 + hiByte) - 2;
	
			byte data[] = new byte[dataLength];
	
			int readSize = 0;
	
			for (int i = 0; i != -1 && readSize < dataLength; readSize += i) 
			{
				i = _from_server.read(data, readSize, dataLength - readSize);
			}
	
			if (readSize != dataLength) 
			{
				_log.warn("Incomplete Packet is sent to the server, closing connection.");
				throw new RuntimeException();
			}
			if (_key_initialized == 1)
			{
				sendPacket(data, 0);
				byte [] decrypted = LineageEncryption.decrypt(data, dataLength, _clkey);
				return decrypted;
			}
			else if (data[0] == 50)
			{	//this packet contains the encryption key
				long add = data[1] & 0xFF;
				_seed += add;
				add = data[2] & 0xFF;
				_seed += add * 0x100;
				add = data[3] & 0xFF;
				_seed += add * 0x10000;
				add = data[4] & 0xFF;
				_seed += add * 0x1000000;
				_log.info("Encryption key: " + _seed);
				initEncryption(_seed);
				printBuffer(data);
				sendPacket(data, 0);
			}
	//		_log.info("Receive " + decrypted[0] + " from " + getAccountName() + ":" + _hostname);
			//forward to server (but only after we get the encryption key)
			return data;
		}
		catch (IOException e) 
		{
				throw e;
		}
	}


	
	public void run()
	{	//here is where packets will be forwarded from client to server
		while (_done == 0)
		{
			try 
			{ 
				byte [] buffer = readPacket();
				printBuffer(buffer);
				//you may now process the packet
			}  
			catch (Exception e) 
			{
				 _log.warn(e.getLocalizedMessage());
				 _parent.closes();
				 _done = 1;
			} 
		}
	}
}

