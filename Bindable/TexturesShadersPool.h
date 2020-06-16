#pragma once

#include "System/Rendering.h"

#include <memory>
#include <unordered_map>
#include <type_traits>


class Texture;
class TextureArray;
class PixelShader;
class VertexShader;
class GeometryShader;

class TexturesShadersPool
{
public:
	template<class T, typename...Params>
	static std::shared_ptr<T> Resolve(Rendering& renderer, Params&&... params) noexcept
	{
		return Get().Resolve_<T>(renderer, std::forward<Params>(params)...);
	}

private:

	TexturesShadersPool() = default;

	template<class T, typename ... Params>
	std::shared_ptr<T> Resolve_(Rendering& renderer, Params&& ... params) noexcept
	{
		
		auto key = T::GenerateUID(std::forward<Params>(params)...);

		std::unordered_map<std::string,std::shared_ptr<T>>* pool;

		typename std::unordered_map<std::string, std::shared_ptr<T>>::iterator it;

		if constexpr (std::is_same<T, VertexShader>::value)
		{
			it = vertexShaderPool.find(key);
			pool = &vertexShaderPool;
		}
		else if constexpr (std::is_same<T, PixelShader>::value)
		{
			it = pixelShaderPool.find(key);
			pool = &pixelShaderPool;
		}
		else if constexpr (std::is_same<T, GeometryShader>::value)
		{
			it = geometryShaderPool.find(key);
			pool = &geometryShaderPool;
		}
		else if constexpr (std::is_same<T, Texture>::value)
		{
			it = texturePool.find(key);
			pool = &texturePool;
		}
		else if constexpr (std::is_same<T, TextureArray>::value)
		{
			it = textureArrayPool.find(key);
			pool = &textureArrayPool;
		}
		

		if (it == pool->end())
		{
			auto bind = std::make_shared<T>(renderer, std::forward<Params>(params)...);
			
			pool->operator[](key) = bind;
			return bind;
		}
		else
		{
			return it->second;
		}

	}
	static TexturesShadersPool& Get()
	{
		static TexturesShadersPool codex;
		return codex;
	}
	
private:
	std::unordered_map<std::string, std::shared_ptr<PixelShader>> pixelShaderPool;
	std::unordered_map<std::string, std::shared_ptr<VertexShader>> vertexShaderPool;
	std::unordered_map<std::string, std::shared_ptr<GeometryShader>> geometryShaderPool;
	std::unordered_map<std::string, std::shared_ptr<Texture>> texturePool;
	std::unordered_map<std::string, std::shared_ptr<TextureArray>> textureArrayPool;

	
};
