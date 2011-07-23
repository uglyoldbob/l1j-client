// DES Encryption and Decryption
// from efgh.com/software
#ifndef __DES_H_
#define __DES_H_

typedef unsigned long DWORD;

const int DES_KEY_SIZE		=	56;
const int DES_DATA_SIZE 	=	64;
const int DES_SBUFFER_SIZE	=	48;
const int DES_ROUNDS =	16;

#ifdef TRIPLEDES
	class triple_des;
#endif

class des
{
	private:
		const char *xmix(const char *, unsigned char [DES_DATA_SIZE],
			const unsigned char [DES_KEY_SIZE]);
		void encrypt_decrypt(unsigned char [DES_DATA_SIZE], int /* boolean */);
		unsigned char compressed_shifted_key[DES_ROUNDS][DES_SBUFFER_SIZE];

	public:
		void initialize(const unsigned char[DES_KEY_SIZE]);
		void password(const char *);
		void key(const char *);
		void encrypt_block(unsigned char *buf, int size);
		void decrypt_block(unsigned char *buf, int size);
		void encrypt(unsigned char data[DES_DATA_SIZE])
		{
			encrypt_decrypt(data, 1 /* true */);
		}
		void decrypt(unsigned char data[DES_DATA_SIZE])
		{
			encrypt_decrypt(data, 0 /* false */);
		}
#ifdef TRIPLEDEX
		friend triple_des;
#endif
};

#ifdef TRIPLEDES

class triple_des
{
	private:
	des part[3];
	void encrypt_decrypt(unsigned char [DES_DATA_SIZE], int /* boolean */);
	public:
	void initialize(const unsigned char key[3*DES_KEY_SIZE])
	{
		part[0].initialize(key);
		part[1].initialize(key + DES_KEY_SIZE);
		part[2].initialize(key + 2*DES_KEY_SIZE);
	}
	void password(const char *);
	void encrypt(unsigned char data[DES_DATA_SIZE])
	{
		part[0].encrypt(data);
		part[1].decrypt(data);
		part[2].encrypt(data);
	}
	void decrypt(unsigned char data[DES_DATA_SIZE])
	{
		part[2].decrypt(data);
		part[1].encrypt(data);
		part[0].decrypt(data);
	}
};

#endif

#endif