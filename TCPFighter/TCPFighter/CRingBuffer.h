#ifndef __CRING_BUFFER__
#define __CRING_BUFFER__

#define BUFFER_SIZE		1024

class CRingBuffer
{
public:
	CRingBuffer(void);
	CRingBuffer(int  iBufferSize);

	~CRingBuffer(void);

public:
	void Resize(int iSize);
	int GetBufferSize(void);
	int GetUseSize(void);
	int GetFreeSize(void);

	int DirectEnqueueSize(void);
	int DirectDequeueSize(void);

	bool Enqueue(char* chpData, int iSize);
	bool Dequeue(char* chpData, int iSize);
	bool Peek(char* chpData, int iSize);

	void MoveRear(int iSize);
	void MoveFront(int iSize);
	void ClearBuffer(void);

	char* GetFrontBufferPtr(void);
	char* GetRearBufferPtr(void);

	int NextIndex(int idx);

	//testCode
	int GetRearPos();
	int GetFrontPos();
private:
	char* m_queue;
	int		m_iBufferSize;
	int		m_iRear;
	int		m_iFront;
};

#endif