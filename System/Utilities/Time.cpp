#include "Time.h"
#include <sstream>
//#include <iomanip>
#include "src/thirdParty/ImGUI/imgui.h"
#include "Console.h"

Time::Time()
{
	last = std::chrono::steady_clock::now();
	start = std::chrono::high_resolution_clock::now();
}

void Time::OnUpdate()
{
	const auto old = last;
	last = std::chrono::high_resolution_clock::now();
	dTime = (std::chrono::duration<float>(last - old)).count();
}

Benchmark::Benchmark(std::string && title)
	: title(title), start(std::chrono::high_resolution_clock::now())
{
	
}

Benchmark::~Benchmark()
{
	using namespace std::string_literals;
	Console::PrintInfo(title + " took: "s + std::to_string((std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - start)).count() * 1000.f) + " ms");
} 
