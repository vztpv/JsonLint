

#define _CRT_SECURE_NO_WARNINGS

//#include <vld.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>


#include "wiz/ClauText.h""


#define ENTER '\n' // cf) std::endl

using namespace std::literals;


namespace Lint {
	bool cout_off = false; // print yes or no?
	wiz::Option opt;

	// justOne + optional_ -> no ok?
	// onemore + optional_ -> no ok?
	class Option
	{
	public:
		enum class Type_ { ANY, INT, FLOAT, STRING, DATETIME, DATETIME_A, DATETIME_B };

		enum class Id_ { NONE, ID, TOTAL_ID };
		enum class OneMore_ { NONE, ONEMORE, JUSTONE };
		enum class Required_ { REQUIRED, OPTIONAL_ };
		enum class EmptyUT_ { NONE, OFF, ON };

		enum class Order_ { NONE, OFF, ON }; // with global??
		enum class Multiple_ { NONE, OFF, ON };
	public:
		std::vector<Type_> type;
		Id_ id;
		OneMore_ onemore;
		EmptyUT_ empty_ut;
		Required_ required;
		std::vector<std::string> event_ids;

		std::string prefix;
	public:
		Option() : type(), id(Id_::NONE), 
			onemore(OneMore_::NONE), 
			required(Required_::REQUIRED),
			empty_ut(EmptyUT_::NONE)
		{
			//
		}
	public:
		Option& Type(Type_ type) {
			this->type.push_back(type);
			return *this;
		}
		Option& Id(Id_ id) {
			this->id = id;
			return *this;
		}
		Option& OneMore(OneMore_ onemore) {
			this->onemore = onemore;
			return *this;
		}
		Option& EmptyUT(EmptyUT_ empty_ut) {
			this->empty_ut = empty_ut;
			return *this;
		}
		Option& Required(Required_ required) {
			this->required = required;
			return *this;
		}
		Option& Event(const std::string& event_id) {
			this->event_ids.push_back(event_id);
			return *this;
		}
	public:
		// check valid function?
		bool IsValid() const {
			// to do
			return false;
		}
	};


	Option OptionFrom(const std::string& option_str)
	{
		Option option;
		std::string::size_type start = 0;
		std::string::size_type find_percent = std::string::npos; // % : delimeter.

		if ((find_percent = option_str.find('%', start)) == std::string::npos)
		{
			option.prefix = option_str;
		}
		else {
			option.prefix = option_str.substr(start, find_percent - start);
		}

		while ((find_percent = option_str.find('%', start)) != std::string::npos)
		{
			std::string::size_type end_ = option_str.find('%', find_percent + 1);

			start = find_percent; // ex) abc%id?

			if (end_ == std::string::npos) {
				end_ = option_str.size();
			}

			const std::string opt = option_str.substr(start, end_ - 1 - start + 1);

			if ("%int"sv == opt) {
				option.Type(Option::Type_::INT);
			}
			else if ("%float"sv == opt) {
				option.Type(Option::Type_::FLOAT);
			}
			else if ("%string"sv == opt) {
				option.Type(Option::Type_::STRING);
			}
			else if ("%datetime"sv == opt) {
				option.Type(Option::Type_::DATETIME);
			}
			else if ("%datetime_a"sv == opt) {
				option.Type(Option::Type_::DATETIME_A);
			}
			else if ("%datetime_b"sv == opt) {
				option.Type(Option::Type_::DATETIME_B);
			}
			else if ("%id"sv == opt) {
				option.Id(Option::Id_::ID);
			}
			else if ("%total_id"sv == opt) {
				option.Id(Option::Id_::TOTAL_ID);
			}
			else if ("%any"sv == opt) {
				option.Type(Option::Type_::ANY);
			}
			else if ("%one_more"sv == opt) { // x = { 1 2 3 4  } -> x = { %int%one_more%event_plus_test }
				option.OneMore(Option::OneMore_::ONEMORE);
			}
			else if ("%just_one"sv == opt) {
				option.OneMore(Option::OneMore_::JUSTONE);
			}
			else if ("%optional"sv == opt) {
				option.Required(Option::Required_::OPTIONAL_);
			}
			else if ("%required"sv == opt) {
				option.Required(Option::Required_::REQUIRED);
			}
			else if ("%can_empty_ut"sv == opt) {
				option.EmptyUT(Option::EmptyUT_::ON);
			}
			else if (wiz::String::startsWith(opt, "%event_")) { // size check?
				std::string event_name = wiz::String::substring(opt, 7);
				option.Event(std::move(event_name));
			}
			else {
				std::cout << "wrong option" << ENTER;
				exit(-2);
			}

			start = end_;
		}

		if (option.type.empty()) {
			option.type.push_back(Option::Type_::ANY);
		}
		return option;
	}

	// chk valid in here!
	inline bool OptionDoA(const Option& option, std::string_view str)
	{
		if (option.prefix.empty() == false &&
			option.prefix == str) {
			return true;
		}
		else if (option.prefix.empty() == false &&
			option.prefix != str) {
			return false;
		}

		long long count = option.type.size();
		auto type_list = option.type;

		while (!type_list.empty()) {
			switch (type_list.back()) {
			case Option::Type_::ANY:
				// pass
				break;
			case Option::Type_::INT:
				if (wiz::load_data::Utility::IsInteger(str)) {
					//
				}
				else {
					std::cout << str << " is not integer" << ENTER;
					count--;
				}
				break;
			case Option::Type_::FLOAT:
				if (wiz::load_data::Utility::IsDouble(str)) {
					//
				}
				else {
					std::cout << str << " is not double" << ENTER;
					count--;
				}
				break;
			case Option::Type_::DATETIME:
				if (wiz::load_data::Utility::IsDate(str)) {
					//
				}
				else {
					std::cout << str << " is not date" << ENTER;
					count--;
				}
				break;
			case Option::Type_::DATETIME_A:
				if (wiz::load_data::Utility::IsDateTimeA(str)) {
					//
				}
				else {
					std::cout << str << " is not datetime A" << ENTER;
					count--;
				}
				break;
			case Option::Type_::DATETIME_B:
				if (wiz::load_data::Utility::IsDateTimeB(str)) {
					//
				}
				else {
					std::cout << str << " is not datetime B" << ENTER;
					count--;
				}
				break;
			case Option::Type_::STRING:
				if (str.size() >= 2 && str[0] == str.back() && str.back() == '\"') { // cf '\''
					//
				}
				else {
					std::cout << str << " is not quoted string" << ENTER;
					count--;
				}
				break;
			}
			type_list.pop_back();
		}

		return count > 0;
	}

	std::tuple<bool, Option, Option> _Check(wiz::load_data::UserType* schema_eventUT,
		const wiz::load_data::ItemType<WIZ_STRING_TYPE>& x, const wiz::load_data::ItemType<WIZ_STRING_TYPE>& y, const std::string& real_dir) //, Order?
	{
		const Option var_option = OptionFrom(x.GetName().ToString()); // name, value check - not start with % ??
		const Option val_option = OptionFrom(x.Get(0).ToString());

		// val only case, ex) A = { a b c d } , a, b, c, d `s name is empty.
		if (x.GetName().ToString().empty()) {
			if (!y.GetName().ToString().empty()) {
				//
				return{ false, var_option, val_option };
			}
		}
		else { // bug?
			if (y.GetName().ToString().empty()) {
				//
				return{ false, var_option, val_option };
			}
		}

		// option type check.
		const bool name_do = OptionDoA(var_option, y.GetName().ToString());
		const bool val_do = OptionDoA(val_option, y.Get(0).ToString());

		if (name_do && val_do) {
			// event check.
			wiz::ClauText clauText;

			std::string event_name;

			for (auto& x : var_option.event_ids) {
				event_name = x;
				// for var // chk no start with __, no end with __ ?
				wiz::load_data::LoadData::AddData(*schema_eventUT, "/./",
					"Event = { id = __" + event_name + "__ $call = { id = " + event_name +
					" name = " + y.GetName().ToString() + " value = " + y.Get(0).ToString() +
					" is_usertype = FALSE " +
					" real_dir = " + real_dir +
					" select = NAME " +
					" input = " + y.GetName().ToString() +
					" } }",
					wiz::ExecuteData());

				if ("TRUE"sv == clauText.excute_module("Main = { $call = { id = __" + event_name + "__ } }", schema_eventUT, wiz::ExecuteData(), opt, 1)) {
					schema_eventUT->RemoveUserTypeList(schema_eventUT->GetUserTypeListSize() - 1);
				}
				else {
					schema_eventUT->RemoveUserTypeList(schema_eventUT->GetUserTypeListSize() - 1);

					std::cout << "clauText is not valid1" << ENTER;
					return { false, var_option, val_option };
				}
			}
			for (auto& x : val_option.event_ids) {
				event_name = x;

				// for val
				wiz::load_data::LoadData::AddData(*schema_eventUT, "/./",
					"Event = { id = __" + event_name + "__ $call = { id = " + event_name +
					" name = " + y.GetName().ToString() + " value = " + y.Get(0).ToString() +
					" is_usertype = FALSE " +
					" real_dir = " + real_dir +
					" select = VALUE " +
					" input = " + y.Get(0).ToString() +
					" } }",
					wiz::ExecuteData());

				if ("TRUE"sv == clauText.excute_module("Main = { $call = { id = __" + event_name + "__ } }", schema_eventUT, wiz::ExecuteData(), opt, 1)) {
					schema_eventUT->RemoveUserTypeList(schema_eventUT->GetUserTypeListSize() - 1);
				}
				else {
					schema_eventUT->RemoveUserTypeList(schema_eventUT->GetUserTypeListSize() - 1);

					std::cout << "clauText is not valid2" << ENTER;
					return { false, var_option, val_option };
				}
			}
		}
		else {
			return { false, var_option, val_option };
		}

		return { true, var_option, val_option };
	}

	std::tuple<bool, Option> _Check(wiz::load_data::UserType* schema_eventUT,
		const wiz::load_data::UserType* x, const wiz::load_data::UserType* y, const std::string& real_dir // Order?
	)
	{
		Option var_option = OptionFrom(x->GetName().ToString()); // name, value check - not start with % ??

		// val only case, ex) A = { a b c d } , a, b, c, d `s name is empty.
		if (x->GetName().ToString().empty()) {
			if (!y->GetName().ToString().empty()) {
				//
				return{ false, var_option };
			}
		}
		else { // bug?
			if (!y || y->GetName().ToString().empty()) {
				//
				return{ false, var_option };
			}
		}

		// option type check.
		const bool name_do = OptionDoA(var_option, y->GetName().ToString());

		if (name_do) {
			// event check.
			wiz::ClauText clauText;
			std::string event_name;

			for (auto& x : var_option.event_ids) {
				event_name = x;

				// for var // chk no start with __, no end with __ ?
				wiz::load_data::LoadData::AddData(*schema_eventUT, "/./",
					"Event = { id = __" + event_name + "__ $call = { id = " + event_name +
					" name = " + y->GetName().ToString() +
					" is_usertype = TRUE " +
					" real_dir = " + real_dir +
					" select = NAME " +
					" input = " + y->GetName().ToString() +
					"}  } ",
					wiz::ExecuteData());

				if ("TRUE"sv == clauText.excute_module("Main = { $call = { id = __" + event_name + "__ } }", schema_eventUT, wiz::ExecuteData(), opt, 1)) {
					schema_eventUT->RemoveUserTypeList(schema_eventUT->GetUserTypeListSize() - 1);
				}
				else {
					schema_eventUT->RemoveUserTypeList(schema_eventUT->GetUserTypeListSize() - 1);

					std::cout << "clauText is not valid3" << ENTER;
					return { false, var_option };
				}
			}
		}
		else {
			return { false, var_option };
		}

		return { true, var_option };
	}

	// varaible!
	std::set<std::tuple<std::string, std::string, std::string>> check_total_id;

	bool Check(wiz::load_data::UserType* schema_eventUT, wiz::load_data::UserType* schemaUT,
		wiz::load_data::UserType* clautextUT, int depth, bool& log_on, bool is_optional = false)
	{


		return true;
	}

	inline bool _Validate(const wiz::load_data::UserType& schemaFileUT)
	{
		wiz::load_data::UserType* clautextUT;
		wiz::load_data::UserType schema_eventUT = schemaFileUT;
		wiz::load_data::UserType* schemaUT;

		// __init__ first init.
		std::string start_name;
		std::string schema_name;
		{
			wiz::ClauText clauText;
			std::string str = clauText.excute_module("Main = { $call = { id = __init__ } }", &schema_eventUT, wiz::ExecuteData(), opt, 1); // 0 (remove events) -> 1 (revoke events?)

			{
				wiz::load_data::UserType temp;
				wiz::load_data::LoadData::LoadDataFromString(str, temp);

				if (temp.GetItemListSize() != 2) {
					std::cout << "data, schema is needed\n";
					return false;
				}

				start_name = temp.GetItemList(0).Get().ToString();
				schema_name = temp.GetItemList(1).Get().ToString();
			}
		}

		clautextUT = wiz::load_data::UserType::Find(&schema_eventUT, start_name).second[0];
		schemaUT = wiz::load_data::UserType::Find(&schema_eventUT, schema_name).second[0];


		// for log?
		bool log_on = false;
		const bool chk = Check(&schema_eventUT, schemaUT, clautextUT, 0, log_on);

		{
			wiz::ClauText clauText;

			clauText.excute_module("Main = { $call = { id = __end__ } }", &schema_eventUT, wiz::ExecuteData(), opt, 1); // 0 (remove events) -> 1 (revoke events?)
		}

		//// debug
		//std::cout << schema_eventUT.ToString() << ENTER
		//	<< schemaUT.ToString() << ENTER;
		return chk;
	}

	inline bool Validate(const wiz::load_data::UserType& schema)
	{
		return _Validate(schema);
	}


	// to do
	//std::pair<bool, std::string> MakeSchema(const std::string& clautext)
	//{
	//	// to do
	//}


	inline std::pair<bool, wiz::load_data::UserType> LoadFile(const std::string& fileName)
	{
		wiz::load_data::UserType ut;

		bool valid = wiz::load_data::LoadData::LoadDataFromFile(fileName, ut, 0, 0);

		return { valid, (ut) };
	}
	inline std::pair<bool, wiz::load_data::UserType> LoadJsonFile(const std::string& fileName)
	{
		wiz::load_data::UserType ut;

		bool valid = wiz::load_data::LoadData2::LoadDataFromFile(fileName, ut, 0, 0);

		return { valid, (ut) };
	}

	// todo SaveJsonFile? for Make Schema?
	inline bool SaveFile(const std::string& fileName, wiz::load_data::UserType* data)
	{
		wiz::load_data::LoadData::SaveWizDB(*data, fileName, "1");
		return true;
	}


}

int main(int argc, char* argv[])
{
	std::string option;
	wiz::load_data::UserType schema; // argv[3]?
	wiz::load_data::UserType clautext; // argv[2]
	std::string fileName; // to save

	try {
		// -v : version? - to do
		// -V : Validate.
		// -M : Make schema. // removal?
		if (argc == 3 && 0 == strcmp("-V", argv[1])) {
			option = "-V";

			//auto chk_clautext = Lint::LoadFile(argv[2]);
			auto chk_schema = Lint::LoadFile(
				//"C:\\Users\\vztpv\\Desktop\\Clau\\ClauText\\c.txt"); //
				argv[2]); // 3 -> 2

			if (chk_schema.first) {
				schema = std::move(chk_schema.second);
			}
			else {
				std::cout << "schema load fail" << ENTER;
				return 1;
			}
			//if (chk_clautext.first) {
			//	clautext = std::move(chk_clautext.second);
			//}
			//else {
			//	std::cout << "clautext load fail" << ENTER;
			//	return 2;
			//}
		}
		else if (argc == 4 && 0 == strcmp("-M", argv[1])) {
			option = "-M";

			auto chk_clautext = Lint::LoadFile(argv[2]);

			if (chk_clautext.first) {
				clautext = std::move(chk_clautext.second);
			}
			else {
				std::cout << "clautext load fail" << ENTER;
				return 3;
			}
			fileName = argv[3];
		}

		if (option == "-V") {
			auto chk = Lint::Validate(schema);
			if (chk) {
				std::cout << ENTER << "success" << ENTER;
			}
			else {
				std::cout << ENTER << "fail" << ENTER;
			}
		}
		else if (option == "-M") {
			//	auto chk = MakeSchema(clautext);
			//	if (chk.first) {
			//		schema = chk.second;

			//		if (SaveFile(fileName, schema)) {

			//		}
			//		else {
			//			std::cout << "save fail" << ENTER;
			//			return 4;
			//		}

			//		std::cout << "success" << ENTER;
			//	}
			//	else {
			//		std::cout << "fail" << ENTER;
			//	}
		}
		else {
			std::cout << "it is not valid option" << ENTER;
		}
	}
	catch (const char* str) {
		std::cout << str << "\n";
	}
	//catch (...) {
	//	std::cout << "unknown error\n";
	//}

	return 0;
}


