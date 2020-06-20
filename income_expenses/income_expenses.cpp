// income_expenses.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <numeric>

// 按 delimiter 分割字符串 
bool splite_line(const std::string & strLine, std::vector<std::string> & stringVector, const char & delimiter = '-') {
	unsigned long long nBegin = 0;
	unsigned long long nEnd = 0;
	unsigned int iter = 0;
	bool flag = false;
	std::string strtemp;
	while ((nEnd = strLine.find_first_of(delimiter, nBegin)) != std::string::npos)
	{
		strtemp = strLine.substr(nBegin, nEnd - nBegin);
		stringVector.emplace_back(strtemp);
		nBegin = nEnd + 1;
	}
	if ((nBegin = strLine.find_last_of(delimiter, strLine.length() - 1)) != std::string::npos)
	{
		strtemp = strLine.substr(nBegin + 1, strLine.length() - nBegin - 1);
		stringVector.emplace_back(strtemp);
		flag = true;
	}
	return flag;
}

// 时间存储
class Time {
public:
	int year;
	int month;
	int date;
};

// 获取从当前位置开始的第一个非空格字符
int get_first_character(int index, const std::string & info_str) {
	while (index < info_str.length() && info_str[index] == ' ')
		++index;
	return index < info_str.length() ? index : -1;
}

// 获取从当前位置开始的最后一个非空格字符
int get_end_character(int index, const std::string & info_str) {
	while (index < info_str.length() && info_str[index] != ' ')
		++index;
	return index;
}

// 获取从当前位置开始的最后一个非空格单词 -- 连续的非空格字符串
std::string get_first_word(int & index, const std::string & info_str) {
	int start_index = get_first_character(index, info_str);
	if (start_index == -1)
		return "";
	index = get_end_character(start_index, info_str);
	return info_str.substr(start_index, index - start_index);
}

//支出收入存储&转换类
class Income_Expenses_Info {
	std::vector<std::string> income_str = { "生活费","奖学金","稿费" };
	std::vector<std::string> expense_str = { "学习用品","生活用品","游戏" };
public:
	bool income_expenses; // 支出还是收入
	int num; // 支出或收入的第num类
	Time time; //时间
	double money; //金钱
	std::string info; //备注
	
	// 字符串转换为有用信息
	void operator()(const std::string & info_str) {
		int index = 0;
		auto str = get_first_word(index, info_str);
		std::vector<std::string> str_list;
		while (!str.empty())
		{
			str_list.emplace_back(str);
			str = get_first_word(index, info_str);
		}//字符串分割

		std::stringstream stream;//类型转换专用类

		income_expenses = (str_list[0][0] == 'a');

		stream << str_list[0][1];
		stream >> num;
		stream.clear();

		std::vector<std::string> date_str_vector;
		splite_line(str_list[1], date_str_vector);

		stream << date_str_vector[0];
		stream >> time.year;
		stream.clear();

		stream << date_str_vector[1];
		stream >> time.month;
		stream.clear();

		stream << date_str_vector[2];
		stream >> time.date;
		stream.clear();

		stream << str_list[2];
		stream >> money;
		stream.clear();

		info = str_list[3];
	}

	// 支持打印的友元函数
	friend std::ostream &operator<<(std::ostream &cout, Income_Expenses_Info &info) {
		cout << (info.income_expenses ? "收入" : "支出") << ' ';
		cout << (info.income_expenses ? info.income_str[info.num - 1] : info.expense_str[info.num - 1]) << ' ';
		cout << info.time.year << '-' << info.time.month << '-' << info.time.date << ' ';
		cout << info.money << ' ';
		cout << info.info;
		return cout;
	}

	// 支持文件保存的友元函数
	friend std::ofstream &operator<<(std::ofstream &cout, Income_Expenses_Info &info) {
		cout << (info.income_expenses ? 'a' : 'b');
		cout << info.num << ' ';
		cout << info.time.year << '-' << info.time.month << '-' << info.time.date << ' ';
		cout << info.money << ' ';
		cout << info.info;
		return cout;
	}
};

// 提示信息专用类
class Recorder {
public:
	// 打印提示信息
	void show_info() {
		std::cout << "收入类:";
		int i = 0;
		for (auto info = income.begin(); info < income.end() - 1; ++info) {
			std::cout << 'a' << ++i << '-' << *info << ',';
		}
		std::cout << 'a' << ++i << '-' << income.back() << '\n';

		std::cout << "支出类:";
		i = 0;
		for (auto info = expenses.begin(); info < expenses.end() - 1; ++info) {
			std::cout << 'b' << ++i << '-' << *info << ',';
		}
		std::cout << 'b' << ++i << '-' << expenses.back();
	}
private:
	std::vector<std::string> income = { "生活费","奖学金","稿费" };
	std::vector<std::string> expenses = { "学习用品","生活用品","游戏" };
};

// 将内存数据保存至文件，文件名为filename
void info_list_to_file(const std::string & filename, std::vector<Income_Expenses_Info> & info_list) {
	std::ofstream cout = std::ofstream(filename.data(), std::ios::out | std::ios::trunc);
	for (auto info = info_list.begin(); info < info_list.end() - 1; ++info) {
		cout << *info << '\n';
	}
	cout << info_list.back();
	cout.close();
}

// 将文件读取至内存，文件名为filename
void file_to_info_list(const std::string & filename, std::vector<Income_Expenses_Info> & info_list) {
	bool convert = false;
	std::ifstream Reader(filename.data());
	if (!Reader) {
		throw (std::invalid_argument("filename is invalid!"));
	}
	info_list.clear();
	Income_Expenses_Info get_info;
	std::string input;
	while (getline(Reader, input)) {
		if (input.size() == 0)
			break;
		get_info(input);
		info_list.emplace_back(get_info);
	}
	Reader.close();
}

// 信息统计类
class Info_Statistics {
	bool income_expenses;
	int num;

	Time time;

	bool is_time = true;
	std::vector<std::string> income_str = { "生活费","奖学金","稿费" };
	std::vector<std::string> expense_str = { "学习用品","生活用品","游戏" };
	std::vector<double> income;
	std::vector<double> expense;
	std::vector<Income_Expenses_Info> info_list_statistics;

public:

	Info_Statistics() {
		for (size_t i = 0; i < income_str.size(); i++)
		{
			income.push_back(0);
		}

		for (size_t i = 0; i < expense_str.size(); i++)
		{
			expense.push_back(0);
		}
	}

	//  打印历史收支明细
	void show_history() {
		if(is_time)
			std::cout << time.year << " 年 " << time.month << " 月收支类别数据的明细\n";
		std::cout << "类别 收入 / 支出 发生日期 金额 备注\n";
		for (auto & info : info_list_statistics) {
			std::cout << info << '\n';
		}
	}

	//  获取符合时间段的收支明细
	void get_from_time(const std::string & input, std::vector<Income_Expenses_Info> info_list) {
		is_time = true;
		std::vector<std::string> str_vector;
		splite_line(input, str_vector, '-');
		std::stringstream stream;
		stream << str_vector[0];
		stream >> time.year;
		stream.clear();

		stream << str_vector[1];
		stream >> time.month;
		info_list_statistics.clear();
		for (size_t i = 0; i < income.size(); i++)
		{
			income[i] = 0;
		}

		for (size_t i = 0; i < expense.size(); i++)
		{
			expense[i] = 0;
		}
		
		for (const auto & info : info_list) { // 穷举全部的信息
			if (info.time.year == time.year &&
				info.time.month == time.month) { // 对比年月
				info_list_statistics.emplace_back(info); // 存储有用信息
				if (info.income_expenses) {
					if (income.size() < info.num) {
						income.emplace_back(info.money);
					}
					else {
						income[info.num - 1] += info.money;
					}
				}
				else {
					if (expense.size() < info.num) {
						expense.emplace_back(info.money);
					}
					else {
						expense[info.num - 1] += info.money;
					}
				}
			}
		}
	}

	//  获取符合类别的收支明细
	void get_from_category(const std::string & input, std::vector<Income_Expenses_Info> info_list) {
		is_time = false;
		int i = 0;
		for (const auto & str : income_str) {
			++i;
			if (input == str) {
				income_expenses = true;
				num = i;
			}
		}
			
		i = 0;
		for (const auto & str : expense_str) {
			++i;
			if (input == str) {
				income_expenses = false;
				num = i;
			}
		}
		
		info_list_statistics.clear();
		for (size_t i = 0; i < income.size(); i++)
		{
			income[i] = 0;
		}

		for (size_t i = 0; i < expense.size(); i++)
		{
			expense[i] = 0;
		}
		for (const auto & info : info_list) {
			if (info.income_expenses == income_expenses &&
				info.num == num) {
				info_list_statistics.emplace_back(info);
				if (info.income_expenses) {
					if (income.size() < info.num) {
						income.emplace_back(info.money);
					}
					else {
						income[info.num - 1] += info.money;
					}
				}
				else {
					if (expense.size() < info.num) {
						expense.emplace_back(info.money);
					}
					else {
						expense[info.num - 1] += info.money;
					}
				}
			}
		}
	}

	// 统计信息打印有元函数
	friend std::ostream &operator<<(std::ostream &cout, Info_Statistics &info) {
		if(info.is_time)
			cout << info.time.year << " 年 " << info.time.month << " 月收支类别数据的汇总\n";
		cout << "收入 / 支出 明细类别 金额\n";
		for (int index = 0; index < info.income.size(); index++) {
			cout << "收入 " << info.income_str[index] << ' ' << info.income[index] << '\n';
		}

		for (int index = 0; index < info.expense.size(); index++) {
			cout << "支出 " << info.expense_str[index] << ' ' << info.expense[index] << '\n';
		}

		double income_sum = std::accumulate(info.income.begin(), info.income.end(), 0);
		double expense_sum = std::accumulate(info.expense.begin(), info.expense.end(), 0);

		cout << info.time.year << " 年 " << info.time.month << " 月总收入为 : " << income_sum << ", 总支出为 : " << expense_sum;
		return cout;
	}

};

int main()
{
	Income_Expenses_Info get_info;
	std::vector<Income_Expenses_Info> info_list;

	//提示信息
	Recorder recorder;
	recorder.show_info();
	std::cout << '\n';

	//信息输入
	std::string input;
	std::cout << "输入收支明细 : ";
	while (getline(std::cin, input)) {
		if (input.size() == 0)
			break;
		get_info(input);
		info_list.emplace_back(get_info);
		std::cout << "输入收支明细 : ";
	}

	//保存到文件
	info_list_to_file("log", info_list);

	//读取文件到内存
	file_to_info_list("log", info_list);

	//查询信息
	Info_Statistics info_statistics;

	//查询信息按时间
	std::cout << "请输入对收支类别数据进行汇总的月份 : ";
	while (getline(std::cin, input)) {
		if (input.size() == 0)
			break;
		info_statistics.get_from_time(input, info_list);
		std::cout << info_statistics << '\n';
		std::cout << "是否输出该月的各笔明细（y 为输出，其他为不输出）: ";
		std::string right;
		std::cin >> right;
		if (right == "y") {
			info_statistics.show_history();
		}
		std::cout << "请输入对收支类别数据进行汇总的月份 : ";
		std::cin.ignore();
	}

	//查询信息按类别
	std::cout << "请输入需要查询的类别 : ";
	while (getline(std::cin, input)) {
		if (input.size() == 0)
			break;
		info_statistics.get_from_category(input, info_list);
		std::cout << info_statistics << '\n';
		std::cout << "是否输出该月的各笔明细（y 为输出，其他为不输出）: ";
		std::string right;
		std::cin >> right;
		if (right == "y") {
			info_statistics.show_history();
		}
		std::cout << "请输入需要查询的类别 : ";
		std::cin.ignore();
	}

	return 0;
}