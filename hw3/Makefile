all:
	gcc server.c -o server
	gcc file_reader.c -o file_reader
run:
	./hw4 -data ../data -output ./submission.csv -tree 100 -thread 4
remove:
	rm -f server file_reader
