#include "RecvBuffer.h"

RecvBuffer::RecvBuffer()
{
	
}

RecvBuffer::~RecvBuffer()
{
}

void RecvBuffer::RecvBufferInit(int32 bufferSize)
{
	buffer_size_ = (bufferSize);
	capacity_ = bufferSize * BUFFER_COUNT;
	buffer_.resize(bufferSize);
}

void RecvBuffer::Clean()
{
	int32 data_size = DataSize();

	if (data_size == 0)
	{
		// 딱 마침 읽기 + 쓰기 커서거 동일 위치라서 위치 리셋 조정
		read_pos_ = write_pos_ = 0;
	}
	else
	{
		// 여유 공간이 버퍼 1개 크기 미만이면 데이터 앞으로 이동
		if (FreeSize() < buffer_size_)
		{
			// 뒤가 더이상 쓸곳이 없어서 앞쪽으로 데이터 복사
			::memcpy(&buffer_[0], &buffer_[read_pos_], data_size);
			// read, write 위치 앞쪽으로 변경
			read_pos_ = 0;
			write_pos_ = data_size;
		}
		
	}
}

bool RecvBuffer::OnRead(int32 numOfBytes)
{
	if (numOfBytes > DataSize())
	{
		return false;
	}
	read_pos_ += numOfBytes;
	return true;
}

bool RecvBuffer::OnWrite(int32 numOfBytes)
{
	if (numOfBytes > FreeSize())
	{
		return false;
	}

	read_pos_ += numOfBytes;
	return true;
}
