/*
*	@file Shader.h
*	
*	Shader Stuff, parser to external files etc...
*/

#pragma once
#include "Core/Core.h"
#include <string>
#include <cstdint>
#include <unordered_map>
#include "glm/glm.hpp"

typedef unsigned int GLenum;
namespace Game
{

	class Shader
	{
	private:
		uint32_t m_Id = NULL;
		std::string m_Name = "";
		std::string m_Path = "";
		std::unordered_map<GLenum, std::string> m_GL_SourceCode;
		bool disposed = false;
	public:

		Shader(const std::string& path);
		Shader(const std::string& name, const std::string& vs, const std::string& fs);
		~Shader();

		static Ref<Shader> CreateShader(const std::string& name, const std::string& vs, const std::string& fs);
		static Ref<Shader> CreateShader(const std::string& path);

		static std::string VerifyShaderError(uint32_t shader_id);
		static std::string VerifyProgramError(uint32_t program_id);

		const std::string& GetName() const { return m_Name; };

		void Bind() const;
		void Unbind() const;
		void Dispose();

		void SetUniform1i(const std::string& name, int32_t value);
		void SetUniform1iv(const std::string& name, int32_t size, int32_t* value);
		void SetUniform1f(const std::string& name, float_t value);
		void SetUniform1fv(const std::string& name, int32_t size, float_t* value);
		void SetUniform2f(const std::string& name, float_t v0, float_t v1);
		void SetUniform3f(const std::string& name, float_t v0, float_t v1, float_t v2);
		void SetUniform4f(const std::string& name, float_t v0, float_t v1, float_t v2, float_t v3);
		void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);

	private:
		std::string ReadFile(const std::string& path);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);

		void CreateProgram();

		void checkCompileErrors(uint32_t shader, const char* type, const char* path);
		std::string GetShaderFile(const char* path, int32_t MaxTexSlots = 8);
	};

	class ShaderLib
	{
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	public:
		void Add(const std::string& name, const Ref<Shader>& shader);
		void Add(const Ref<Shader>& shader);
		Ref<Shader> Load(const std::string& path);
		Ref<Shader> Load(const std::string& name, const std::string& path);

		Ref<Shader> Get(const std::string& name);

		bool Exists(const std::string& name) const;

		Ref<Shader> operator[](const std::string& name);
	};

}