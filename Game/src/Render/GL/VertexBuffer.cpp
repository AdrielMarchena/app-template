#include "pch.h"
#include "VertexBuffer.h"

#include "glad/glad.h"
#include "gl_error_macro_db.h"
namespace Game
{
	
	VertexBuffer::~VertexBuffer()
	{
		if(m_Id)
			Dispose();
	}
	void VertexBuffer::Bind()
	{
		GAME_CORE_ASSERT(m_Id > 0,"Vertex Buffer id can not be 0");
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_Id));
	}

	void VertexBuffer::Unbind()
	{
		GAME_CORE_ASSERT(m_Id > 0,"Vertex Buffer id can not be 0");
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	void VertexBuffer::Dispose()
	{
		GAME_CORE_ASSERT(m_Id > 0,"Vertex Buffer id can not be 0");
		GLCall(glDeleteBuffers(1, &m_Id));
		m_Id = NULL;
	}

	void VertexBuffer::SubData(size_t size, const void* buffer, int offset)
	{
		GAME_CORE_ASSERT(m_Id > 0,"Vertex Buffer id can not be 0");
		Bind();
		GLCall(glBufferSubData(GL_ARRAY_BUFFER, offset, size, buffer));
	}

	Ref<VertexBuffer> VertexBuffer::CreateVertexBuffer(size_t size, GL_Usage usage)
	{
		Ref<VertexBuffer> new_vb = MakeRef<VertexBuffer>();
		new_vb->m_Usage = usage;
		GLCall(glGenBuffers(1, &new_vb->m_Id));
		GAME_CORE_ASSERT(new_vb->m_Id > 0,"Vertex Buffer id can not be 0");
		new_vb->Bind();
		GLCall(glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_SwitchUsage(usage)));
		return new_vb;
	}
}