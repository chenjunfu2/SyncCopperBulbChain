#include <iostream>
#include <string>
#include <stdint.h>
#include <vector>
#include <algorithm>
#include <ctype.h>
//#include <locale.h>

enum class Mode :uint8_t
{
	SolveProblem = 0,
	SimulationRun,
};

constexpr static const char *const pStrMode[] =
{
	"SolveProblem",
	"SimulationRun",
};

// 判断是否为2的幂
bool IsPowerOf2(int n)
{
	return n > 0 && (n & (n - 1)) == 0;
}

// 计算大于等于x的最小2的幂
uint64_t NextPowerOf2(uint64_t x)
{
	if (x <= 1)
	{
		return 1;
	}

	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x |= x >> 32;
	return ++x;
}

static std::string g_strOutput[2] = { "0 ","1 " };

void OutputBoolList(const std::vector<uint8_t> &boolInput)
{
	for (auto &it : boolInput)
	{
		fwrite(g_strOutput[it].data(), 1, g_strOutput[it].size(), stdout);
	}
}

void SolveProblem(std::vector<uint8_t> &boolInput)
{
	if (boolInput.size() <= 1)
	{
		printf("No Solution!\n");
		return;
	}

	bool bOk = false;
	for (size_t i = 0, iend = boolInput.size(); i < iend; ++i)
	{
		if (boolInput[i] != boolInput[0])
		{
			bOk = true;
			break;
		}
	}

	if (!bOk)
	{
		printf("No Solution!\n");
		return;
	}

	//不足2的幂次循环补齐
	size_t iptSize = boolInput.size();
	if (!IsPowerOf2(iptSize))
	{
		auto nextSize = NextPowerOf2(iptSize);
		for (size_t i = 0; i < nextSize - iptSize; ++i)
		{
			boolInput.push_back(boolInput[i]);
		}

		iptSize = boolInput.size();
	}


	std::vector<uint8_t> boolSolve;

	//求十进制位数
	size_t sDigits = (size_t)log10((iptSize - 1)) + 1;

	//原始行直接输出
	boolSolve.push_back(boolInput[0]);
	printf("[%0*zu]: ", sDigits, iptSize - 1);
	OutputBoolList(boolInput);
	putchar('\n');

	for (size_t i = 0; i < iptSize - 1; ++i)
	{
		//计算
		bool bAllFalse = true;
		for (size_t j = 0, jend = boolInput.size(); j + 1 < jend; ++j)
		{
			//XNOR
			boolInput[j] = !(boolInput[j] ^ boolInput[j + 1]);
			bAllFalse = bAllFalse && !boolInput[j];
		}
		boolInput.pop_back();//裁切末尾

		if (bAllFalse)
		{
			break;
		}

		//进行输出
		boolSolve.push_back(boolInput[0]);
		printf("[%0*zu]: ", sDigits, iptSize - i - 2);
		OutputBoolList(boolInput);
		putchar('\n');
	}

	std::reverse(boolSolve.begin(), boolSolve.end());
	printf("[Solved]: ");
	OutputBoolList(boolSolve);
	putchar('\n');
}

void SimulationRun(std::vector<uint8_t> &boolInput)
{
	size_t szRunCount = NextPowerOf2(boolInput.size());
	unsigned int uiDigits = (size_t)log10((szRunCount - 1)) + 1;
	std::vector<uint8_t> biInitialState = boolInput;

	//输出一次初始状态，代表循环开始
	printf("[%0*zu]: ", uiDigits, (size_t)0);
	OutputBoolList(biInitialState);
	putchar('\n');

	//运算中间态
	for (size_t c = 0, cend = szRunCount; c < cend; ++c)
	{
		bool bLast = false;//让第一次直接命中
		for (auto &it : boolInput)
		{
			bool bTmp = it;
			if (!bLast)
			{
				it = !bTmp;
			}
			bLast = bTmp;
		}

		//输出
		printf("[%0*zu]: ", uiDigits, c + 1);
		OutputBoolList(boolInput);
		putchar('\n');
	}

	//最终输出一次初始状态，代表循环完成
	printf("[%0*zu]: ", uiDigits, szRunCount + 1);
	OutputBoolList(biInitialState);
	putchar('\n');
}

void Help(void)
{
	printf("Use:\nS    - Solve Problem\nR    - Simulation Run\nM    - Modify Output Format\nH    - Help\nQ    - Quit\n0/1  - Input Data\n");
}

int main(void)
{
	Mode enMode = Mode::SolveProblem;
	std::string strLine;
	std::vector<uint8_t> boolInput;

	//输出改为全缓冲以缓解大型输出问题
	//setlocale(LC_ALL, ".UTF-8");//设置编码
	setvbuf(stdout, NULL, _IOFBF, 4096);

	Help();
	printf("\nDefault Mode Is [%s]\n", pStrMode[(uint8_t)enMode]);
	fflush(stdout);

	while (true)
	{
	Retry:
		printf("\n> ");
		fflush(stdout);

		strLine.clear();
		if (std::getline(std::cin, strLine, '\n').eof())
		{
			return 0;
		}

		if (strLine.empty())
		{
			continue;
		}

		if (strLine.size() == 1)
		{
			char c = strLine[0];
			switch (c)
			{
			case 'S':
			case 's':
				enMode = Mode::SolveProblem;
				printf("Type Change To [%s]\n", pStrMode[(uint8_t)enMode]);
				fflush(stdout);
				continue;
			case 'r':
			case 'R':
				enMode = Mode::SimulationRun;
				printf("Type Change To [%s]\n", pStrMode[(uint8_t)enMode]);
				fflush(stdout);
				continue;
			case 'm':
			case 'M':
				{
					std::string strTemp;
					printf("0: \"%s\" -> ", g_strOutput[0].c_str());
					fflush(stdout);
					if (std::getline(std::cin, strTemp, '\n').eof())
					{
						return 0;
					}
					if (strTemp.empty())
					{
						g_strOutput[0] = "0 ";
						printf("0 Is Modified To Default: \"%s\"\n", g_strOutput[0].c_str());
					}
					else
					{
						g_strOutput[0] = std::move(strTemp);
						printf("0 Is Modified To: \"%s\"\n", g_strOutput[0].c_str());
					}

					printf("1: \"%s\" -> ", g_strOutput[1].c_str());
					fflush(stdout);
					if (std::getline(std::cin, strTemp, '\n').eof())
					{
						return 0;
					}
					if (strTemp.empty())
					{
						g_strOutput[1] = "1 ";
						printf("1 Is Modified To Default: \"%s\"\n", g_strOutput[1].c_str());
					}
					else
					{
						g_strOutput[1] = std::move(strTemp);
						printf("1 Is Modified To: \"%s\"\n", g_strOutput[1].c_str());
					}

					printf("Output Modified Success!\n");
					fflush(stdout);
				}
				continue;
			case 'h':
			case 'H':
				Help();
				fflush(stdout);
				continue;
			case 'q':
			case 'Q':
				return 0;
			case '0':
			case '1':
				break;
			default:
				printf("Unknown Type[%c](0x%02X)!\n", c, c);
				Help();
				fflush(stdout);
				continue;
			}
		}

		boolInput.clear();
		boolInput.reserve(strLine.size());
		for (auto &c : strLine)
		{
			switch (c)
			{
			case '0':
				boolInput.push_back(0);
				break;
			case '1':
				boolInput.push_back(1);
				break;
			default:
				if (isspace(c))//跳过空白
				{
					continue;
				}

				printf("Unknown Char[%c](0x%02X)!\n", c, c);
				fflush(stdout);
				goto Retry;
			}
		}

		//进行迭代
		switch (enMode)
		{
		case Mode::SolveProblem:
			SolveProblem(boolInput);
			fflush(stdout);
			break;
		case Mode::SimulationRun:
			SimulationRun(boolInput);
			fflush(stdout);
			break;
		default:
			printf("Fatal Error!\n");
			fflush(stdout);
			exit(-1);
			break;
		}
	}

	return 0;
}

