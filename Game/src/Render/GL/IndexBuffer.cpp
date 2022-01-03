#include "pch.h"
#include "IndexBuffer.h"

#include "glad/glad.h"
#include "gl_error_macro_db.h"
namespace Game
{

	IndexBuffer::~IndexBuffer()
	{
		if (m_Id)
			Dispose();
	}

	void IndexBuffer::Bind()
	{
		GAME_CORE_ASSERT(m_Id > 0,"");
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id));
	}

	void IndexBuffer::Unbind()
	{
		GAME_CORE_ASSERT(m_Id > 0,"");
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}

	void IndexBuffer::Dispose()
	{
		GAME_CORE_ASSERT(m_Id > 0,"");
		GLCall(glDeleteBuffers(1, &m_Id));
		m_Id = NULL;
	}

	Ref<IndexBuffer> IndexBuffer::CreateIndexBuffer(size_t size, uint32_t* indices, GL_Usage usage)
	{
		Ref<IndexBuffer> new_ib = MakeRef<IndexBuffer>();
		new_ib->m_Usage = usage;
		GLCall(glGenBuffers(1, &new_ib->m_Id));
		GAME_CORE_ASSERT(new_ib->m_Id > 0,"");
		new_ib->Bind();
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_SwitchUsage(usage)));
		return new_ib;
	}
}