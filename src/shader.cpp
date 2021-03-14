#include "shader.h"
//#include "logger.h"

#include <fstream>
#include <sstream>

namespace bullseye::shader {
	Shader::Shader(std::string _name) {
		name = _name;
	}

	inline std::string Shader::load_file(const char* path) {
		std::ifstream input_file(path);
		if (input_file) {
			std::stringstream buffer;
			buffer << input_file.rdbuf();

			return buffer.str();
		}
		else {
			// TODO:
			// Including logger.h here causes link error (symbol already defined), find out how to use it
			//bullseye::logger::error("Failed loading vertex shader!");
			printf("Failed loading vertex shader!");

			return std::string();
		}
	}

	void Shader::load_vertex_shader(const char* path) {
		std::string shader_src = load_file(path);

		vertex_shader_src = shader_src.c_str();
	}

	void Shader::load_fragment_shader(const char* path) {
		std::string shader_src = load_file(path);

		fragment_shader_src = load_file(path);
	}

	const char* Shader::get_vertex_shader_src() {
		return vertex_shader_src.c_str();
	}

	const char* Shader::get_fragment_shader_src() {
		return fragment_shader_src.c_str();
	}
}

