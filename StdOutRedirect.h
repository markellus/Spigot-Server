//based on https://stackoverflow.com/questions/573724/how-can-i-redirect-stdout-to-some-visible-display-in-a-windows-application/573762

#ifndef STDOUTREDIRECT_H
#define STDOUTREDIRECT_H

#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>

#define READ_FD 0
#define WRITE_FD 1

#define CHECK(a) if ((a)!= 0) return -1;

class StdOutRedirect
{
public:
	StdOutRedirect(int bufferSize)
	{
		if (_pipe(fdStdOutPipe, bufferSize, O_TEXT) != 0)
		{
			//treat error eventually
		}
		fdStdOut = _dup(_fileno(stdout));
	}

	~StdOutRedirect()
	{
		_close(fdStdOut);
		_close(fdStdOutPipe[WRITE_FD]);
		_close(fdStdOutPipe[READ_FD]);
	}

	int start()
	{
		fflush(stdout);
		CHECK(_dup2(fdStdOutPipe[WRITE_FD], _fileno(stdout)));
		std::ios::sync_with_stdio();
		setvbuf(stdout, NULL, _IONBF, 0); // absolutely needed
		return 0;
	}

	int stop() const
	{
		CHECK(_dup2(fdStdOut, _fileno(stdout)));
		std::ios::sync_with_stdio();
		return 0;
	}
	int getBuffer(char* buffer, int size)
	{
		int nOutRead = _read(fdStdOutPipe[READ_FD], buffer, size);
		buffer[nOutRead] = '\0';
		return nOutRead;
	}

private:
	int fdStdOutPipe[2];
	int fdStdOut;
};

#endif // STDOUTREDIRECT_H
