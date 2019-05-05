CFLAGS= -Wall -fpic -lm -std=gnu99

client: chatclient.c
	gcc chatclient.c -o client $(CFLAGS)

clean:
	rm -f server client *.o otp_enc_d otp_enc otp_dec otp_dec_d keygen

