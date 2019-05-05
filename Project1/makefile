CFLAGS= -Wall -fpic -lm -std=gnu99

client: client.c
	gcc client.c -o client $(CFLAGS)

server: server.c
	gcc server.c -o server $(CFLAGS)

clientserver: clean client server prog4_lib.c


keygen: keygen.c
	gcc keygen.c -o keygen $(CFLAGS)


test: test.c prog4_lib.h
	rm -f test
	gcc test.c -o test $(CFLAGS)

otp_dec_d: otp_dec_d.c prog4_lib.c server.c
	gcc otp_dec_d.c -o otp_dec_d $(CFLAGS)


otp_dec: prog4_lib.c otp_dec.c
	gcc otp_dec.c -o otp_dec $(CFLAGS)

otp_enc_d: otp_enc_d.c prog4_lib.c server.c
	gcc otp_enc_d.c -o otp_enc_d $(CFLAGS)


otp_enc: prog4_lib.c otp_enc.c
	gcc otp_enc.c -o otp_enc $(CFLAGS)



project: otp_enc_d otp_enc otp_dec_d otp_dec keygen

test_otp: otp_enc otp_enc_d
clean:
	rm -f server client *.o otp_enc_d otp_enc otp_dec otp_dec_d keygen

