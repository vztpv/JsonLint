

#define _CRT_SECURE_NO_WARNINGS

//#include <vld.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>


#include "wiz/ClauText.h"


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
		enum class Type_ { ANY, INT, FLOAT, NUMBER, STRING, BOOLEAN, NULL_, };
		enum class Id_ { NONE, ID };
		enum class Required_ { REQUIRED, OPTIONAL_ };
		enum class Multiple_ { NONE, OFF, ON };
	public:
		std::vector<Type_> type;
		Id_ id;
		Required_ required;

		std::vector<std::string> event_ids;
		std::vector<std::string> enum_ids;
		std::vector<std::string> style_ids;

		std::string prefix;
	public:
		Option() : type(), id(Id_::NONE), 
			required(Required_::REQUIRED)
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
		Option& Required(Required_ required) {
			this->required = required;
			return *this;
		}
		Option& Event(const std::string& event_id) {
			this->event_ids.push_back(event_id);
			return *this;
		}
		Option& Enum(const std::string& enum_id) {
			this->enum_ids.push_back(enum_id);
			return *this;
		}
		Option& Style(const std::string& style_id) {
			this->style_ids.push_back(style_id);
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
			else if ("%number"sv == opt) {
				option.Type(Option::Type_::NUMBER);
			}
			else if ("%bool"sv == opt) {
				option.Type(Option::Type_::BOOLEAN);
			}
			else if ("%null"sv == opt) {
				option.Type(Option::Type_::NULL_);
			}
			else if ("%string"sv == opt) {
				option.Type(Option::Type_::STRING);
			}
			else if ("%id"sv == opt) {
				option.Id(Option::Id_::ID);
			}
			else if ("%any"sv == opt) {
				option.Type(Option::Type_::ANY);
			}
			else if ("%optional"sv == opt) {
				option.Required(Option::Required_::OPTIONAL_);
			}
			else if ("%required"sv == opt) {
				option.Required(Option::Required_::REQUIRED);
			}
			else if (wiz::String::startsWith(opt, "%event_")) { // size check?
				std::string event_name = wiz::String::substring(opt, 7);
				option.Event(std::move(event_name));
			}
			else if (wiz::String::startsWith(opt, "%enum_")) { // size check?
				std::string enum_name = wiz::String::substring(opt, 6);
				option.Enum(std::move(enum_name));
			}
			else if (wiz::String::startsWith(opt, "%style_")) {
				option.Style(wiz::String::substring(opt, 7));
			}
			else {
				std::cout << "wrong option" << ENTER;
				exit(-2); // throw -2?
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
				if (wiz::load_data::Utility::IsIntegerInJson(str)) {
					//
				}
				else {
					std::cout << str << " is not integer" << ENTER;
					count--;
				}
				break;
			case Option::Type_::FLOAT:
				if (wiz::load_data::Utility::IsFloatInJson(str)) {
					//
				}
				else {
					std::cout << str << " is not float" << ENTER;
					count--;
				}
				break;
			
			case Option::Type_::NUMBER:
				if (wiz::load_data::Utility::IsNumberInJson(str)) {
					//
				}
				else {
					std::cout << str << "is not number(integer + float)" << ENTER;
					count--;
				}
				break;
			case Option::Type_::BOOLEAN:
				if (str != "true" && str != "false") {
					std::cout << str << "is not boolean" << ENTER;
					count--;
				}
				break;
			case Option::Type_::NULL_:
				if (str != "null") {
					std::cout << str << "is not null" << ENTER;
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

	std::tuple<bool, Option, Option> _Check(wiz::load_data::UserType* mainUT, const std::map<std::string, wiz::load_data::UserType*>& enumMap,
		const std::map<std::string, wiz::load_data::UserType*>& styleMap,
		const wiz::load_data::ItemType<WIZ_STRING_TYPE>& x, const wiz::load_data::ItemType<WIZ_STRING_TYPE>& y, const std::string& real_dir,
		std::vector<std::string>* name_style_vec, std::vector<std::string>* val_style_vec) //, Order?
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
		const bool name_do = (val_style_vec == nullptr)? true : OptionDoA(var_option, y.GetName().ToString());
		const bool val_do = (name_style_vec == nullptr)? true : OptionDoA(val_option, y.Get(0).ToString());


		if (name_do && val_do) {
			// event check.
			wiz::ClauText clauText;

			std::string event_name;

			// event
			for (auto& x : var_option.event_ids) {
				event_name = x;
				// for var // chk no start with __, no end with __ ?
				wiz::load_data::LoadData::AddData(*mainUT, "/./",
					"Event = { id = __" + event_name + "__ $call = { id = " + event_name +
					" name = " + y.GetName().ToString() + " value = " + y.Get(0).ToString() +
					" is_usertype = FALSE " +
					" real_dir = " + real_dir +
					" select = NAME " +
					" input = " + y.GetName().ToString() +
					" } }",
					wiz::ExecuteData());

				if ("TRUE"sv == clauText.excute_module("Main = { $call = { id = __" + event_name + "__ } }", mainUT, wiz::ExecuteData(), opt, 1)) {
					mainUT->RemoveUserTypeList(mainUT->GetUserTypeListSize() - 1);
				}
				else {
					mainUT->RemoveUserTypeList(mainUT->GetUserTypeListSize() - 1);

					std::cout << "clauText is not valid1" << ENTER;
					return { false, var_option, val_option };
				}
			}
			for (auto& x : val_option.event_ids) {
				event_name = x;

				// for val
				wiz::load_data::LoadData::AddData(*mainUT, "/./",
					"Event = { id = __" + event_name + "__ $call = { id = " + event_name +
					" name = " + y.GetName().ToString() + " value = " + y.Get(0).ToString() +
					" is_usertype = FALSE " +
					" real_dir = " + real_dir +
					" select = VALUE " +
					" input = " + y.Get(0).ToString() +
					" } }",
					wiz::ExecuteData());

				if ("TRUE"sv == clauText.excute_module("Main = { $call = { id = __" + event_name + "__ } }", mainUT, wiz::ExecuteData(), opt, 1)) {
					mainUT->RemoveUserTypeList(mainUT->GetUserTypeListSize() - 1);
				}
				else {
					mainUT->RemoveUserTypeList(mainUT->GetUserTypeListSize() - 1);

					std::cout << "clauText is not valid2" << ENTER;
					return { false, var_option, val_option };
				}
			}

			// enum 
		// ex)
		//	Enum = {
		//		id = x

		//		1 2 3 4
		//	  }
			for (auto& id : var_option.enum_ids) {
				auto enumUT = enumMap.at(id);

				bool found = false;
				size_t start = 0, last = enumUT->GetItemListSize() - 1;
				while (start <= last) {
					size_t middle = (start + last) / 2; // chk overflow...
					auto middle_item = enumUT->GetItemList(middle).Get();
					int diff = middle_item.ToString().compare(y.GetName().ToString());

					if (diff == 0) {
						found = true;
						break;
					}
					else if (diff < 0) {
						start = middle + 1;
					}
					else {
						if (0 == middle) {
							break;
						}
						last = middle - 1;
					}
					middle = (start + last) / 2; //
				}
				if (!found) {
					std::cout << "clauText is not valid2.5" << ENTER;
					return { false, var_option, val_option };
				}
			}
			for (auto& id : val_option.enum_ids) {
				auto enumUT = enumMap.at(id);

				bool found = false;
				size_t start = 0, last = enumUT->GetItemListSize() - 1;
				while (start <= last) {
					size_t middle = (start + last) / 2; // chk overflow...
					auto middle_item = enumUT->GetItemList(middle).Get();
					int diff = middle_item.ToString().compare(y.GetName().ToString());

					if (diff == 0) {
						found = true;
						break;
					}
					else if (diff < 0) {
						start = middle + 1;
					}
					else {
						if (0 == middle) {
							break;
						}
						last = middle - 1;
					}
					middle = (start + last) / 2; //
				}
				if (!found) {
					std::cout << "clauText is not valid2.5" << ENTER;
					return { false, var_option, val_option };
				}
			}

			// style
		//	Style = {
	//			id = x
//
		//		%multiple
		//		%optional
		//		%event_int_string = { }
	//			%event_list_sum_larger_than = { n = /./x }
		//	}

			if (name_style_vec) {
				std::sort(name_style_vec->begin(), name_style_vec->end());

				std::string result;
				auto styles = *name_style_vec;
				for (auto& id : var_option.style_ids) {
					if (std::binary_search(styles.begin(), styles.end(), id)) {
						throw "clauText Schema`s style is wrong.";
					}

					auto styleUT = styleMap.at(id);
					styles.push_back(id);

					for (size_t i = 0; i < styleUT->GetItemListSize(); ++i) {
						result += styleUT->GetItemList(i).Get().ToString();
					}

					for (size_t i = 0; i < styleUT->GetUserTypeListSize(); ++i) {
						result += styleUT->GetUserTypeList(i)->GetName().ToString();
						wiz::load_data::UserType temp = *(styleUT->GetUserTypeList(i));

						result += "@\'" + wiz::load_data::LoadData::ToBool4(nullptr, *mainUT, temp, wiz::ExecuteData()).ToString();
						result += "\'";
					}
				}
				if (!std::get<0>(_Check(mainUT, enumMap, styleMap, wiz::load_data::ItemType<WIZ_STRING_TYPE>("", result), y, real_dir,
					&styles, nullptr))) {
					std::cout << "clauText is not valid2.6" << ENTER;
					return { false, var_option, val_option };
				}
			}
			if (val_style_vec) {
				std::sort(val_style_vec->begin(), val_style_vec->end());

				std::string result;
				auto styles = *val_style_vec;
				for (auto& id : val_option.style_ids) {
					if (std::binary_search(styles.begin(), styles.end(), id)) {
						throw "clauText Schema`s style is wrong.2.65";
					}

					auto styleUT = styleMap.at(id);
					styles.push_back(id);

					for (size_t i = 0; i < styleUT->GetItemListSize(); ++i) {
						result += styleUT->GetItemList(i).Get().ToString();
					}

					for (size_t i = 0; i < styleUT->GetUserTypeListSize(); ++i) {
						result += styleUT->GetUserTypeList(i)->GetName().ToString();
						wiz::load_data::UserType temp = *(styleUT->GetUserTypeList(i));

						result += "@\'" + wiz::load_data::LoadData::ToBool4(nullptr, *mainUT, temp, wiz::ExecuteData()).ToString();
						result += "\'";
					}
				}
				if (!std::get<0>(_Check(mainUT, enumMap, styleMap, wiz::load_data::ItemType<WIZ_STRING_TYPE>("", result), y, real_dir,
					nullptr, &styles))) {
					std::cout << "clauText is not valid2.7" << ENTER;
					return { false, var_option, val_option };
				}
			}
		}
		else {
			return { false, var_option, val_option };
		}

		return { true, var_option, val_option };
	}

	std::tuple<bool, Option> _Check(wiz::load_data::UserType* mainUT, const std::map<std::string, wiz::load_data::UserType*>& enumMap,
		const std::map<std::string, wiz::load_data::UserType*>& styleMap,
		const wiz::load_data::UserType& x, const wiz::load_data::UserType& y, 
		const std::string& real_dir, std::vector<std::string>* name_style_vec
	)
	{
		Option var_option = OptionFrom(x.GetName().ToString()); // name, value check - not start with % ??

		// val only case, ex) A = { a b c d } , a, b, c, d `s name is empty.
		if (x.GetName().ToString().empty()) {
			if (!y.GetName().ToString().empty()) {
				//
				return{ false, var_option };
			}
		}
		else { // bug?
			if (y.GetName().ToString().empty()) {
				//
				return{ false, var_option };
			}
		}

		// option type check.
		const bool name_do = OptionDoA(var_option, y.GetName().ToString());

		if (name_do) {
			// event check.
			wiz::ClauText clauText;
			std::string event_name;

			for (auto& x : var_option.event_ids) {
				event_name = x;

				// for var // chk no start with __, no end with __ ?
				wiz::load_data::LoadData::AddData(*mainUT, "/./",
					"Event = { id = __" + event_name + "__ $call = { id = " + event_name +
					" name = " + y.GetName().ToString() +
					" is_usertype = TRUE " +
					" real_dir = " + real_dir +
					" select = NAME " +
					" input = " + y.GetName().ToString() +
					"}  } ",
					wiz::ExecuteData());

				if ("TRUE"sv == clauText.excute_module("Main = { $call = { id = __" + event_name + "__ } }", mainUT, wiz::ExecuteData(), opt, 1)) {
					mainUT->RemoveUserTypeList(mainUT->GetUserTypeListSize() - 1);
				}
				else {
					mainUT->RemoveUserTypeList(mainUT->GetUserTypeListSize() - 1);

					std::cout << "clauText is not valid3" << ENTER;
					return { false, var_option };
				}
			}

			for (auto& id : var_option.enum_ids) {
				auto enumUT = enumMap.at(id);

				bool found = false;
				size_t start = 0, last = enumUT->GetItemListSize() - 1;
				while (start <= last) {
					size_t middle = (start + last) / 2; // chk overflow...
					auto middle_item = enumUT->GetItemList(middle).Get();
					int diff = middle_item.ToString().compare(y.GetName().ToString());

					if (diff == 0) {
						found = true;
						break;
					}
					else if (diff < 0) {
						start = middle + 1;
					}
					else {
						if (0 == middle) {
							break;
						}
						last = middle - 1;
					}
					middle = (start + last) / 2; //
				}
				if (!found) {
					std::cout << "clauText is not valid3.5" << ENTER;
					return { false, var_option };
				}
			}

			{
				std::sort(name_style_vec->begin(), name_style_vec->end());

				std::string result;
				auto styles = *name_style_vec;
				for (auto& id : var_option.style_ids) {
					if (std::binary_search(styles.begin(), styles.end(), id)) {
						throw "clauText Schema`s style is wrong.3.55";
					}

					auto styleUT = styleMap.at(id);
					styles.push_back(id);

					for (size_t i = 0; i < styleUT->GetItemListSize(); ++i) {
						result += styleUT->GetItemList(i).Get().ToString();
					}

					for (size_t i = 0; i < styleUT->GetUserTypeListSize(); ++i) {
						result += styleUT->GetUserTypeList(i)->GetName().ToString();
						wiz::load_data::UserType temp = *(styleUT->GetUserTypeList(i));

						result += "@\'" + wiz::load_data::LoadData::ToBool4(nullptr, *mainUT, temp, wiz::ExecuteData()).ToString();
						result += "\'";
					}
				}
				if (!std::get<0>(_Check(mainUT, enumMap, styleMap, wiz::load_data::UserType(result), y, real_dir,
					&styles))) {
					std::cout << "clauText is not valid3.6" << ENTER;
					return { false, var_option };
				}
			}
		}
		else {
			return { false, var_option };
		}

		return { true, var_option };
	}

	bool Check(wiz::load_data::UserType* mainUT, wiz::load_data::UserType* schemaUT,
		wiz::load_data::UserType* clautextUT, int depth, bool& log_on, bool is_optional = false)
	{
		// init
		std::map<std::string, wiz::load_data::UserType*> styleMap;
		std::map<std::string, wiz::load_data::UserType*> enumMap;

		{
			auto temp = mainUT->GetUserTypeItem("Style");
			for (size_t i = 0; i < temp.size(); ++i) {
				std::string name = temp[i]->GetItem("id")[0].ToString();
				styleMap.insert(std::make_pair(std::move(name), temp[i]));
			}
		}

		{
			auto temp = mainUT->GetUserTypeItem("Enum");
			for (size_t i = 0; i < temp.size(); ++i) {
				std::string name = temp[i]->GetItem("id")[0].ToString();
				styleMap.insert(std::make_pair(std::move(name), temp[i]));
			}
		}

		// loop

		// test
		std::cout << "chk " << wiz::load_data::Utility::Equal("abc def", "abc\u0021def") << ENTER;


		return true;
	}

	inline bool _Validate(const wiz::load_data::UserType& _mainUT)
	{
		wiz::load_data::UserType* clautextUT;
		wiz::load_data::UserType mainUT = _mainUT;
		wiz::load_data::UserType* schemaUT;

		// __init__ first init.
		std::string start_name;
		std::string schema_name;
		{
			wiz::ClauText clauText;
			std::string str = clauText.excute_module("Main = { $call = { id = __init__ } }", &mainUT, wiz::ExecuteData(), opt, 1); // 0 (remove events) -> 1 (revoke events?)

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

		clautextUT = wiz::load_data::UserType::Find(&mainUT, start_name).second[0];
		schemaUT = wiz::load_data::UserType::Find(&mainUT, schema_name).second[0];


		// for log?
		bool log_on = false;
		const bool chk = Check(&mainUT, schemaUT, clautextUT, 0, log_on);

		{
			wiz::ClauText clauText;

			clauText.excute_module("Main = { $call = { id = __end__ } }", &mainUT, wiz::ExecuteData(), opt, 1); // 0 (remove events) -> 1 (revoke events?)
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


