

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
		enum class Required_ { REQUIRED, OPTIONAL_ };
		enum class Multiple_ { NONE, OFF, ON };
	public:
		std::vector<Type_> type;
		std::vector<std::string> ids;
		Required_ required;
		Multiple_ multiple;

		std::vector<std::string> event_ids;
		std::vector<std::string> enum_ids;
		std::vector<std::string> style_ids;

		std::string prefix;

		wiz::DataType minimum, maximum;
		bool use_minimum = false, use_maximum = false;
		long long minItems = 0, maxItems = 0;
		bool use_minItems = false, use_maxItems = false;

		std::string regex; 

		std::vector<std::string> needs;
	public:
		Option() : type(), 
			required(Required_::REQUIRED),
			multiple(Multiple_::OFF)
		{
			//
		}
	public:
		Option& Multiple(Multiple_ multiple) {
			this->multiple = multiple;
			return *this;
		}
		Option& Type(Type_ type) {
			this->type.push_back(type);
			return *this;
		}
		Option& Id(const std::string& id) {
			this->ids.push_back(id);
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
	public:
		Option operator+(const Option& other) const {
			Option temp = *this;

			for (size_t i = 0; i < other.event_ids.size(); ++i) {
				temp.event_ids.push_back(other.event_ids[i]);
			}
			for (size_t i = 0; i < other.enum_ids.size(); ++i) {
				temp.enum_ids.push_back(other.enum_ids[i]);
			}
			for (size_t i = 0; i < other.style_ids.size(); ++i) {
				temp.style_ids.push_back(other.style_ids[i]);
			}
			for (size_t i = 0; i < other.ids.size(); ++i) {
				temp.ids.push_back(other.ids[i]);
			}

			if (other.multiple == Multiple_::ON) {
				temp.multiple = Multiple_::ON;
			}


			if (other.required == Required_::OPTIONAL_) {
				temp.required = Required_::OPTIONAL_;
			}
			for (const auto& x : other.type) {
				temp.type.push_back(x);
			}

			if (other.use_minimum) {
				temp.use_minimum = true;
				temp.minimum = other.minimum;
			}
			if (other.use_maximum) {
				temp.use_maximum = true;
				temp.maximum = other.maximum;
			}
			
			if (other.use_minItems) {
				temp.use_minItems = true;
				temp.minItems = other.minItems;
			}
			if (other.use_maxItems) {
				temp.use_maxItems = true;
				temp.maxItems = other.maxItems;
			}
			if (!other.regex.empty()) {
				temp.regex = other.regex;
			}

			for (size_t i = 0; i < other.needs.size(); ++i) {
				temp.needs.push_back(other.needs[i]);
			}

			return temp;
		}
	};


	// itemtype, usertype flag?
	Option OptionFrom(const std::string& option_str, wiz::load_data::UserType* mainUT, std::vector<std::string>& style_vec, const std::map<std::string, wiz::load_data::UserType*>& styleMap)
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

			if (wiz::String::startsWith(opt, "%integer"sv)) {
				option.Type(Option::Type_::INT);

				std::vector<std::string> temp = wiz::tokenize(opt, '@');

				if (temp.size() > 1) {
					std::string argument = temp[1].substr(1, temp[1].size() - 2);
					wiz::load_data::UserType ut;
					wiz::load_data::LoadData::LoadDataFromString(argument, ut);

					auto item1 = ut.GetItem("minimum");
					if (item1.size() > 0) {
						option.use_minimum = true;
						option.minimum = item1[0].Get();
					}
					auto item2 = ut.GetItem("maximum");
					if (item2.size() > 0) {
						option.use_maximum = true;
						option.maximum = item2[0].Get();
					}
				}
			}
			else if (wiz::String::startsWith(opt, "%float"sv)) {
				option.Type(Option::Type_::FLOAT);

				std::vector<std::string> temp = wiz::tokenize(opt, '@');

				if (temp.size() > 1) {
					std::string argument = temp[1].substr(1, temp[1].size() - 2);
					wiz::load_data::UserType ut;
					wiz::load_data::LoadData::LoadDataFromString(argument, ut);

					auto item1 = ut.GetItem("minimum");
					if (item1.size() > 0) {
						option.use_minimum = true;
						option.minimum = item1[0].Get();
					}
					auto item2 = ut.GetItem("maximum");
					if (item2.size() > 0) {
						option.use_maximum = true;
						option.maximum = item2[0].Get();
					}
				}
			}
			else if (wiz::String::startsWith(opt, "%number"sv)) {
				option.Type(Option::Type_::NUMBER);
				
				std::vector<std::string> temp = wiz::tokenize(opt, '@');

				if (temp.size() > 1) {
					std::string argument = temp[1].substr(1, temp[1].size() - 2);
					wiz::load_data::UserType ut;
					wiz::load_data::LoadData::LoadDataFromString(argument, ut);

					auto item1 = ut.GetItem("minimum");
					if (item1.size() > 0) {
						option.use_minimum = true;
						option.minimum = item1[0].Get();
					}
					auto item2 = ut.GetItem("maximum");
					if (item2.size() > 0) {
						option.use_maximum = true;
						option.maximum = item2[0].Get();
					}
				}
			}
			else if ("%boolean"sv == opt) {
				option.Type(Option::Type_::BOOLEAN);
			}
			else if ("%null"sv == opt) {
				option.Type(Option::Type_::NULL_);
			}
			else if ("%string"sv == opt) {
				option.Type(Option::Type_::STRING);
			}
			else if (wiz::String::startsWith(opt, "%id_")) {
				option.Id(wiz::String::substring(opt, 4));
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
			else if ("%multiple"sv == opt) {
				option.Multiple(Option::Multiple_::ON);
			}
			else if (wiz::String::startsWith(opt, "%minItems"sv)) { // size check?
				std::string argument;

				{
					std::vector<std::string> temp = wiz::tokenize(opt, '@');
					if (temp.size() > 1) {
						argument = temp[1].substr(1, temp[1].size() - 2);

						wiz::DataType x(argument);
						option.use_minItems = true;
						option.minItems = x.ToInt();
					}
				}

			}
			else if (wiz::String::startsWith(opt, "%maxItems"sv)) { // size check?
				std::string argument;

				{
					std::vector<std::string> temp = wiz::tokenize(opt, '@');
					if (temp.size() > 1) {
						argument = temp[1].substr(1, temp[1].size() - 2);

						wiz::DataType x(argument);
						option.use_maxItems = true;
						option.maxItems = x.ToInt();
					}
				}
			}
			else if (wiz::String::startsWith(opt, "%regex"sv)) {
				std::string expr;

				std::vector<std::string> temp = wiz::tokenize(opt, '@');
				if (temp.size() > 1) {
					expr = temp[1].substr(1, temp[1].size() - 2);

					option.regex = expr;
				}
			}
			else if (wiz::String::startsWith(opt, "%need"sv)) {
				std::string name;

				std::vector<std::string> temp = wiz::tokenize(opt, '@');

				if (temp.size() > 1) {
					name = temp[1].substr(1, temp[1].size() - 2);

					option.needs.push_back(name);
				}
			}
			else if (wiz::String::startsWith(opt, "%event_"sv)) { // size check?
				std::string event_name = wiz::String::substring(opt, 7);
				option.Event(std::move(event_name));
			}
			else if (wiz::String::startsWith(opt, "%enum_"sv)) { // size check?
				std::string enum_name = wiz::String::substring(opt, 6);
				option.Enum(std::move(enum_name));
			}
			else if (wiz::String::startsWith(opt, "%style_"sv)) {
				std::string style_id;
				std::string argument;
				wiz::ExecuteData executeData;

				{
					std::vector<std::string> temp = wiz::tokenize(opt.substr(7), '@');
					style_id = temp[0];
					if (temp.size() > 1) {
						argument = temp[1].substr(1, temp[1].size() - 2);
					}

					wiz::DataType x = wiz::load_data::LoadData::ToBool4(nullptr, *mainUT, argument, wiz::ExecuteData()).ToString();
					wiz::load_data::UserType ut;
					wiz::load_data::LoadData::LoadDataFromString(x.ToString(), ut);

					for (size_t i = 0; i < ut.GetItemListSize(); ++i) {
						executeData.info.parameters.insert({ ut.GetItemList(i).GetName().ToString(), ut.GetItemList(i).Get().ToString() });
					}
				}
				

				std::sort(style_vec.begin(), style_vec.end());

				std::string result;

				if (std::binary_search(style_vec.begin(), style_vec.end(), style_id)) {
					throw "clauText Schema`s style is wrong.3.55";
				}

				auto styleUT = styleMap.at(style_id);
				style_vec.push_back(style_id);

				for (size_t i = 0; i < styleUT->GetItemListSize(); ++i) {
					result += styleUT->GetItemList(i).Get().ToString();
				}

				for (size_t i = 0; i < styleUT->GetUserTypeListSize(); ++i) {
					if (styleUT->GetUserTypeList(i)->GetName() == "$parameter"sv) {
						continue;
					}
					result += styleUT->GetUserTypeList(i)->GetName().ToString();
					wiz::load_data::UserType temp = *(styleUT->GetUserTypeList(i));

					result += "@\'" + wiz::load_data::LoadData::ToBool4(nullptr, *mainUT, temp, executeData).ToString();
					result += "\'";
				}

				option = option + OptionFrom(result, mainUT, style_vec, styleMap);
			}
			else {
				std::cout << "wrong option" << ENTER;
			}

			start = end_;
		}

		if (option.type.empty()) {
			option.type.push_back(Option::Type_::ANY);
		}
		return option;
	}

	inline bool OptionDoA(const Option& option, std::string_view str) // json str
	{
		if (option.prefix.empty() == false &&
			option.prefix == str) {
			return true;
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
					if (option.use_minimum) {
						wiz::DataType x(str.data());
						if (option.minimum.ToInt() <= x.ToInt()) {
							//
						}
						else {
							std::cout << str << "is small" << ENTER;
							count--;
						}
					}
					if (option.use_maximum) {
						wiz::DataType x(str.data());
						if (option.maximum.ToInt() >= x.ToInt()) {
							//
						}
						else {
							std::cout << str << "is big" << ENTER;
							count--;
						}
					}
				}
				else {
					std::cout << str << " is not integer" << ENTER;
					count--;
				}
				break;
			case Option::Type_::FLOAT:
				if (wiz::load_data::Utility::IsFloatInJson(str)) {
					if (option.use_minimum) {
						wiz::DataType x(str.data());
						if (option.minimum.ToFloat() <= x.ToFloat()) {
							//
						}
						else {
							std::cout << str << "is small" << ENTER;
							count--;
						}
					}
					if (option.use_maximum) {
						wiz::DataType x(str.data());
						if (option.maximum.ToFloat() >= x.ToFloat()) {
							//
						}
						else {
							std::cout << str << "is big" << ENTER;
							count--;
						}
					}
				}
				else {
					std::cout << str << " is not float" << ENTER;
					count--;
				}
				break;
			
			case Option::Type_::NUMBER:
				if (wiz::load_data::Utility::IsNumberInJson(str)) {
					if (option.use_minimum) {
						wiz::DataType x(str.data());
						if (option.minimum.ToFloat() <= x.ToFloat()) {
							//
						}
						else {
							std::cout << str << "is small" << ENTER;
							count--;
						}
					}
					if (option.use_maximum) {
						wiz::DataType x(str.data());
						if (option.maximum.ToFloat() >= x.ToFloat()) {
							//
						}
						else {
							std::cout << str << "is big" << ENTER;
							count--;
						}
					}
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
		const std::map<std::string, wiz::load_data::UserType*>& styleMap, std::map<std::string, std::set<std::string>>& idMap, 
		std::map<std::string, std::string>& needMap,
		const wiz::load_data::ItemType<WIZ_STRING_TYPE>& x, const wiz::load_data::ItemType<WIZ_STRING_TYPE>& y, const wiz::load_data::UserType* y_parent, const std::string& real_dir
		) //, Order?
	{
		std::vector<std::string> name_style_vec;
		std::vector<std::string> val_style_vec;

		const Option var_option = OptionFrom(x.GetName().ToString(), mainUT, name_style_vec, styleMap); // name, value check - not start with % ??
		const Option val_option = OptionFrom(x.Get(0).ToString(), mainUT, val_style_vec, styleMap);

		// option type check.
		const bool name_do = OptionDoA(var_option, y.GetName().ToString());
		const bool val_do = OptionDoA(val_option, y.Get(0).ToString());


		if (name_do && val_do) {
			// event check.
			wiz::ClauText clauText;

			std::string event_name;

			// event
			for (auto& x : var_option.event_ids) {
				std::vector<std::string> vec = wiz::tokenize(x, '@');
				event_name = vec[0];
				std::string argument;
				if (vec.size() > 1) {
					argument = vec[1].substr(1, vec[1].size() - 2);
					argument = wiz::load_data::LoadData::ToBool4(nullptr, *mainUT, argument, wiz::ExecuteData()).ToString();
				}

				// for var // chk no start with __, no end with __ ?
				
				std::string func = "Event = { id = __";

				func += event_name;
				func += "__ $call = { id = ";
				func += event_name;
				func += " name = ";
				func += y.GetName().ToString();
				func += " value = ";
				func += y.Get(0).ToString();
				func += " is_usertype = FALSE ";
				func += " real_dir = ";
				func += real_dir;
				func += " select = NAME ";
				func += " input = ";
				func += y.GetName().ToString();
				func += " ";
				func += argument;
				func +=	" } } ";

				wiz::load_data::LoadData::AddData(*mainUT, "/./",
					func,
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
				std::vector<std::string> vec = wiz::tokenize(x, '@');
				event_name = vec[0];
				std::string argument;
				if (vec.size() > 1) {
					argument = vec[1].substr(1, vec[1].size() - 2);
					argument = wiz::load_data::LoadData::ToBool4(nullptr, *mainUT, argument, wiz::ExecuteData()).ToString();
				}

				// for val

				std::string func = "Event = { id = __";

				func += event_name;
				func += "__ $call = { id = ";
				func += event_name;
				func += " name = ";
				func += y.GetName().ToString();
				func += " value = ";
				func += y.Get(0).ToString();
				func += " is_usertype = FALSE ";
				func += " real_dir = ";
				func += real_dir;
				func += " select = VALUE ";
				func += " input = ";
				func += y.Get().ToString();
				func += " ";
				func += argument;
				func += " } } ";

				wiz::load_data::LoadData::AddData(*mainUT, "/./",
					func,
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
					std::cout << "clauText is not valid2.51" << ENTER;
					return { false, var_option, val_option };
				}
			}
			for (auto& id : val_option.enum_ids) {
				auto enumUT = enumMap.at(id);

				bool found = false;

				enumUT->sort_item_list();

				size_t start = 0, last = enumUT->GetItemListSize() - 1;
				while (start <= last) {
					size_t middle = (start + last) / 2; // chk overflow...
					auto middle_item = enumUT->GetItemList(middle).Get();
					int diff = middle_item.ToString().compare(y.Get().ToString());

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
					std::cout << "clauText is not valid2.52" << ENTER;
					return { false, var_option, val_option };
				}
			}



			for (auto& id : var_option.ids) {
				auto ut = idMap.find(id);

				if (ut == idMap.end()) {
					std::cout << "no id fail\n";
					return { false, var_option , val_option };
				}

				auto ut2 = ut->second.find(y.GetName().ToString());
				if (ut2 != ut->second.end()) {
					std::cout << "id is already exist fail\n";
					return { false, var_option , val_option };
				}

				ut->second.insert(y.GetName().ToString());
			}



			for (auto& id : val_option.ids) {
				auto ut = idMap.find(id);

				if (ut == idMap.end()) {
					std::cout << "no id fail\n";
					return { false, var_option, val_option };
				}

				auto ut2 = ut->second.find(y.Get().ToString());
				//std::cout << y.Get().ToString() << "\n";
				if (ut2 != ut->second.end()) {
					std::cout << "id is already exist fail\n";
					return { false, var_option, val_option };
				}

				ut->second.insert(y.Get().ToString());
			}

			if (!var_option.regex.empty()) {
				const std::regex base_regex(var_option.regex);
				std::smatch base_match;
				std::string name = y.GetName().ToString();

				if (std::regex_match(name, base_match, base_regex)) {
					//
				}
				else {
					std::cout << "regex fail\n";
					return { false, var_option, val_option };
				}
			}
			if (!val_option.regex.empty()) {
				const std::regex base_regex(val_option.regex);
				std::smatch base_match;
				std::string value = y.Get().ToString();

				if (std::regex_match(value, base_match, base_regex)) {
					//
				}
				else {
					std::cout << "regex fail\n";
					return { false, var_option, val_option };
				}
			}

			for (size_t i = 0; i < var_option.needs.size(); ++i) {
				std::string old = y.GetName().ToString();
				old = old.substr(1, old.size() - 2);

				if (auto _x = needMap.find(var_option.needs[i]); _x != needMap.end()) {
					while (_x != needMap.end()) {
						if (_x->second == old) {
							std::cout << "need error\n";
							return { false, var_option, val_option };
						}
						_x = needMap.find(_x->second);
					}
				}
				
				needMap.insert({ old, var_option.needs[i] });


				if (y_parent != nullptr) {
					if ((!y_parent->GetItem("\"" + var_option.needs[i] + "\"").empty()) ||
						(!y_parent->GetUserTypeItem("\"" + var_option.needs[i] + "\"").empty())) {
						//
					}
					else {
						std::cout << "need " << ("\"" + var_option.needs[i] + "\"") << "\n";
						return { false, var_option, val_option };
					}
				}
			}
		}
		else {
			return { false, var_option, val_option };
		}

		
		return { true, var_option, val_option };
	}

	std::tuple<bool, Option> _Check(wiz::load_data::UserType* mainUT, const std::map<std::string, wiz::load_data::UserType*>& enumMap,
		const std::map<std::string, wiz::load_data::UserType*>& styleMap, std::map<std::string, std::set<std::string>>& idMap,
		std::map<std::string, std::string>& needMap,
		const wiz::load_data::UserType& x, const wiz::load_data::UserType& y, 
		const std::string& real_dir
	)
	{
		std::vector<std::string> name_style_vec;
		Option var_option = OptionFrom(x.GetName().ToString(), mainUT, name_style_vec, styleMap); // name, value check - not start with % ??

		// option type check.
		const bool name_do = OptionDoA(var_option, y.GetName().ToString());

		if (name_do) {
			{
				if (var_option.use_minItems) {
					if (var_option.minItems <= y.GetIListSize()) {
						//
					}
					else {
						std::cout << "minItems error\n";
						return { false, var_option };
					}
				}
				if (var_option.use_maxItems) {
					if (var_option.maxItems >= y.GetIListSize()) {
						//
					}
					else {
						std::cout << "maxItems error\n";
						return { false, var_option };
					}
				}
			}

			// event check.
			wiz::ClauText clauText;
			std::string event_name;

			for (auto& x : var_option.event_ids) {
				std::vector<std::string> vec = wiz::tokenize(x, '@');
				event_name = vec[0];
				std::string argument;
				if (vec.size() > 1) {
					argument = vec[1].substr(1, vec[1].size() - 2);
					argument = wiz::load_data::LoadData::ToBool4(nullptr, *mainUT, argument, wiz::ExecuteData()).ToString();
				}

				// for var // chk no start with __, no end with __ ?
				wiz::load_data::LoadData::AddData(*mainUT, "/./",
					"Event = { id = __" + event_name + "__ $call = { id = " + event_name +
					" name = " + y.GetName().ToString() +
					" is_usertype = TRUE " +
					" real_dir = " + real_dir +
					" select = NAME " +
					" input = " + y.GetName().ToString() + " " + argument +
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

			for (auto& id : var_option.ids) {
				auto ut = idMap.find(id);
				
				if (ut == idMap.end()) {
					std::cout << "no id fail\n";
					return { false, var_option };
				}

				auto ut2 = ut->second.find(y.GetName().ToString());
				if (ut2 != ut->second.end()) {
					std::cout << "id is already exist fail\n";
					return { false, var_option };
				}

				ut->second.insert(y.GetName().ToString());
			}

			if (!var_option.regex.empty()) {
				const std::regex base_regex(var_option.regex);
				std::smatch base_match;
				std::string name = y.GetName().ToString();

				if (std::regex_match(name, base_match, base_regex)) {
					//
				}
				else {
					std::cout << "regex fail\n";
					return { false, var_option };
				}
			}
			
			for (size_t i = 0; i < var_option.needs.size(); ++i) {
				needMap.insert({ y.GetName().ToString(), var_option.needs[i] });
				std::string old = y.GetName().ToString();
				old = old.substr(1, old.size() - 2);

				if (auto _x = needMap.find(var_option.needs[i]); _x != needMap.end()) {
					while (_x != needMap.end()) {
						if (_x->second == old) {
							std::cout << "need error\n";
							return { false, var_option };
						}
						_x = needMap.find(_x->second);
					}
				}

				needMap.insert({ old, var_option.needs[i] });

				if (y.GetParent() != nullptr) {
					if ((!y.GetParent()->GetItem("\"" + var_option.needs[i] + "\"").empty()) ||
						(!y.GetParent()->GetUserTypeItem("\"" + var_option.needs[i] + "\"").empty())) {
						//
					}
					else {
						std::cout << "need " <<("\"" + var_option.needs[i] + "\"") << "\n";
						return { false, var_option };
					}
				}
			}
		}
		else {
			return { false, var_option };
		}

		return { true, var_option };
	}

	class Wrap {
	public:
		const wiz::load_data::UserType* ut = nullptr;
		size_t idx = 0; // first
		size_t idx_it = 0; // first
		size_t idx_ut = 0; // first
		size_t count = 0; // second
		size_t max = 0; // idx_max
		std::map<std::string, const wiz::load_data::Type*> objectMap; // 

		std::string real_dir;

		std::vector<size_t> it_empty_prefix_vec; // first
		std::vector<size_t> ut_empty_prefix_vec; // first

		std::map<const wiz::load_data::Type*, size_t> visitMap; // second 
		std::queue<size_t> idxVec;
		bool useIdxVec = false;
		bool multiple = false;
	public:
		Wrap(const wiz::load_data::UserType* ut, const std::string& real_dir, wiz::load_data::UserType* mainUT, const std::map<std::string, wiz::load_data::UserType*>& styleMap,
			long long depth, std::map<std::pair<long long, std::string>, Option>* schemaOptionMap = nullptr, 
						std::map<std::pair<long long, std::string>, const wiz::load_data::Type*>* schemaObjectMap = nullptr)	

			: ut(ut), real_dir(real_dir) {

			max = ut->GetIListSize();
			size_t it_count = 0;
			size_t ut_count = 0;

			bool to_insert = false;

			if (schemaOptionMap) {
				for (size_t i = 0; i < ut->GetIListSize(); ++i) {
					if (ut->IsItemList(i)) {
						std::vector<std::string> style_vec;
						Option x;
						if (schemaOptionMap->find({ depth, ut->GetItemList(it_count).GetName().ToString() }) == schemaOptionMap->end()) {
							to_insert = true;
							break;
						}
					}
					else {
						std::vector<std::string> style_vec;
						Option x;
						if (schemaOptionMap->find({ depth, ut->GetUserTypeList(ut_count)->GetName().ToString() }) == schemaOptionMap->end()) {
							to_insert = true;
							break;
						}
					}
				}
			}

			for (size_t i = 0; i < ut->GetIListSize(); ++i) {
				if (ut->IsItemList(i)) {
					std::vector<std::string> style_vec;
					Option x; 
					if (schemaOptionMap) {
						if (to_insert) {
							(*schemaOptionMap)[{depth, ut->GetItemList(it_count).GetName().ToString()}] = OptionFrom(ut->GetItemList(it_count).GetName().ToString(), mainUT, style_vec, styleMap);
						}
						x = schemaOptionMap->at({ depth, ut->GetItemList(it_count).GetName().ToString() });
					}
					else {
						x = OptionFrom(ut->GetItemList(it_count).GetName().ToString(), mainUT, style_vec, styleMap);
					}

					if (!x.prefix.empty() && objectMap.end() != objectMap.find(x.prefix)) {
						std::cout << "duplication\n";
					}
					if (!x.prefix.empty()) {
						objectMap.insert(std::make_pair(x.prefix, &ut->GetItemList(it_count)));
					}
					visitMap.insert({ &ut->GetItemList(it_count), 0 });

					it_count++;
				}
				else {
					std::vector<std::string> style_vec;
					Option x;
					if (schemaOptionMap) {
						if (to_insert) {
							(*schemaOptionMap)[{depth, ut->GetUserTypeList(ut_count)->GetName().ToString()}] = OptionFrom(ut->GetUserTypeList(ut_count)->GetName().ToString(), mainUT, style_vec, styleMap);
						}
						x = schemaOptionMap->at({ depth, ut->GetUserTypeList(ut_count)->GetName().ToString() });
					}
					else {
						x = OptionFrom(ut->GetUserTypeList(ut_count)->GetName().ToString(), mainUT, style_vec, styleMap);
					}
					if (!x.prefix.empty() && objectMap.end() != objectMap.find(x.prefix)) {
						std::cout << "duplication\n";
					}
					if (!x.prefix.empty()) {
						objectMap.insert(std::make_pair(x.prefix, ut->GetUserTypeList(ut_count)));
					}

					visitMap.insert({ ut->GetUserTypeList(ut_count), 0 });

					ut_count++;
				}
			}
		}
	};

	bool Check(wiz::load_data::UserType* mainUT, wiz::load_data::UserType* schemaUT,
		wiz::load_data::UserType* jsonUT, int depth, bool& log_on)
	{
		// init
		std::map<std::string, wiz::load_data::UserType*> styleMap;
		std::map<std::string, wiz::load_data::UserType*> enumMap;
		std::map<std::string, std::string> needMap;
		std::map<std::string, std::set<std::string>> idMap;
		std::map<std::pair<long long, std::string>, Option> schemaOptionMap;

		{
			auto temp = mainUT->GetUserTypeItem("Style");
			for (size_t i = 0; i < temp.size(); ++i) {
				std::string name = temp[i]->GetItem("id")[0].Get().ToString();
				styleMap.insert(std::make_pair(std::move(name), temp[i]));
			}
		}

		{
			auto temp = mainUT->GetUserTypeItem("Enum");
			for (size_t i = 0; i < temp.size(); ++i) {
				std::string name = temp[i]->GetItem("id")[0].Get().ToString();
				enumMap.insert(std::make_pair(std::move(name), temp[i]));
			}
		}

		std::vector<std::pair<Wrap, Wrap>> _stack;
		_stack.push_back({ {schemaUT, "empty_string", mainUT, styleMap, 0}, {jsonUT, "/./" + jsonUT->GetName().ToString(), mainUT, styleMap, 0, &schemaOptionMap}});

		std::vector<std::string> style_vec;

		// loop
		while (!_stack.empty()) {
			if (_stack.back().first.idx >= _stack.back().first.max) {
				if (_stack.back().second.idxVec.empty() && !_stack.back().second.useIdxVec) {
					for (size_t i = 0; i < _stack.back().second.ut->GetItemListSize(); ++i) {
						if (0 == _stack.back().second.visitMap[&(_stack.back().second.ut->GetItemList(i))]) {
							bool pass = false;

							for (size_t idx : _stack.back().first.it_empty_prefix_vec) {
								auto x = _stack.back().first.ut;
								style_vec.clear();
								auto opt = OptionFrom(x->GetItemList(idx).GetName().ToString(), mainUT, style_vec, styleMap);

								if (x->GetItemList(idx).GetName().empty()) {
									style_vec.clear();
									opt = OptionFrom(x->GetItemList(idx).Get().ToString(), mainUT, style_vec, styleMap);
								}

								if (_stack.back().first.visitMap[&x->GetItemList(idx)] != 0 && opt.multiple != Option::Multiple_::ON) {
									continue;
								}

								std::vector<std::string> name_style_vec, val_style_vec;

								if (auto success = _Check(mainUT, enumMap, styleMap, idMap, needMap, x->GetItemList(idx), _stack.back().second.ut->GetItemList(i),
									_stack.back().second.ut,
									_stack.back().second.real_dir); std::get<0>(success)) {
									// std::cout << "success\n";

									pass = true;

									_stack.back().second.count++;

									_stack.back().first.visitMap[&x->GetItemList(idx)]++;
									break;
								}
							}

							if (!pass) {
								std::cout << "fail\n";
								return false;
							}
						}
					}

					for (size_t i = 0; i < _stack.back().second.ut->GetUserTypeListSize(); ++i) {
						if (0 == _stack.back().second.visitMap[_stack.back().second.ut->GetUserTypeList(i)]) {
							bool pass = false;

							for (size_t idx : _stack.back().first.ut_empty_prefix_vec) {
								auto x = _stack.back().first.ut;
								style_vec.clear();
								auto opt = OptionFrom(x->GetUserTypeList(idx)->GetName().ToString(), mainUT, style_vec, styleMap);

								std::string name = _stack.back().second.ut->GetUserTypeList(i)->GetName().ToString();
							//	std::cout << name << "\n";

								if (_stack.back().first.visitMap[x->GetUserTypeList(idx)] != 0 && opt.multiple != Option::Multiple_::ON) {
									continue;
								}

								std::vector<std::string> name_style_vec;
								if (name.empty()) {
									name = "_";
								}

								if (auto success = _Check(mainUT, enumMap, styleMap, idMap, needMap, *(x->GetUserTypeList(idx)), *(_stack.back().second.ut->GetUserTypeList(i)),
									_stack.back().second.real_dir + "/" + name); std::get<0>(success)) {
									// std::cout << "success\n";

									pass = true;

									_stack.back().second.count++;

									_stack.back().second.idxVec.push(i);
									_stack.back().first.idxVec.push(idx);

									_stack.back().first.visitMap[x->GetUserTypeList(idx)]++;

									break;
								}
							}

							if (!pass) {
								std::cout << "fail\n";
								return false;
							}
						}
					}

					for (size_t idx : _stack.back().first.it_empty_prefix_vec) {
						auto x = _stack.back().first.ut;
						style_vec.clear();
						auto opt = OptionFrom(x->GetItemList(idx).GetName().ToString(), mainUT, style_vec, styleMap);

						if (x->GetItemList(idx).GetName().empty()) {
							style_vec.clear();
							opt = OptionFrom(x->GetItemList(idx).Get().ToString(), mainUT, style_vec, styleMap);
						}

						if (_stack.back().first.visitMap[&x->GetItemList(idx)] == 0 && opt.required != Option::Required_::OPTIONAL_) {
							std::cout << "fail\n";
							return false;
						}
					}

					for (size_t idx : _stack.back().first.ut_empty_prefix_vec) {
						auto x = _stack.back().first.ut;
						style_vec.clear();
						auto opt = OptionFrom(x->GetUserTypeList(idx)->GetName().ToString(), mainUT, style_vec, styleMap);

						if (_stack.back().first.visitMap[x->GetUserTypeList(idx)] == 0 && opt.required != Option::Required_::OPTIONAL_) {
							std::cout << "fail\n";
							return false;
						}
					}

				}
				
				if (!_stack.back().second.idxVec.empty()) {
					_stack.back().first.useIdxVec = true;
					_stack.back().second.useIdxVec = true;

						auto temp = _stack.back().second.idxVec.front();
						_stack.back().second.idxVec.pop();
						auto temp2 = _stack.back().first.idxVec.front();
						_stack.back().first.idxVec.pop();

						const wiz::load_data::UserType* nestedJsonUT = _stack.back().second.ut->GetUserTypeList(temp); 
						const wiz::load_data::UserType* nestedSchemaUT = _stack.back().first.ut->GetUserTypeList(temp2);

						std::string name = nestedJsonUT->GetName().ToString();
						_stack.push_back({ { nestedSchemaUT, "empty_string", mainUT, styleMap, (long long)_stack.size() },
						{ nestedJsonUT,
							name.empty() ? _stack.back().second.real_dir + "/_" : _stack.back().second.real_dir + "/" + name, mainUT, styleMap, (long long)_stack.size(), &schemaOptionMap } });
				}
				else {
					_stack.pop_back();
				}
				continue;
			}
			
			auto x = _stack.back().first.ut;
			auto y = _stack.back().second.ut;
			size_t idx = _stack.back().first.idx;
			size_t it_count = _stack.back().first.idx_it;
			size_t ut_count = _stack.back().first.idx_ut;

			if (x->IsItemList(idx)) {
				if ("$log"sv == x->GetItemList(it_count).GetName()) {
					if ("on"sv == x->GetItemList(it_count).Get()) {
						log_on = true;
					}
					else { // off
						log_on = false;
					}
				}
				else if (wiz::String::startsWith(x->GetItemList(it_count).Get().ToString(), "$clear_id_")) {
					std::string temp = x->GetItemList(it_count).Get().ToString().substr(10);
					idMap[temp].clear();
				}
				else {
					style_vec.clear();

					auto opt = OptionFrom(x->GetItemList(it_count).GetName().ToString(), mainUT, style_vec, styleMap);
					std::string key = opt.prefix;
					
					if (!key.empty()) {
						auto result = _stack.back().second.objectMap.find(key);
						if (result != _stack.back().second.objectMap.end()) {
							std::vector<std::string> name_style_vec, val_style_vec;

							if (result->second->IsItemType()) {
								if (auto success = _Check(mainUT, enumMap, styleMap, idMap, needMap, x->GetItemList(it_count), *((wiz::load_data::ItemType<WIZ_STRING_TYPE>*)(result->second)),
									_stack.back().second.ut,
									_stack.back().second.real_dir); std::get<0>(success)) {
									// std::cout << "success\n";

									_stack.back().second.visitMap[result->second] = 1;

									_stack.back().second.count++;
								}
								else {
									if (_stack.back().first.multiple) {
										std::cout << "failed\n";
										return false;
									}
									else {
										std::cout << "failed\n";
										return false;
									}
								}
							}
							else {
								std::cout << "failed\n";
								return false;
							}
						}
						else {
							if (opt.required == Option::Required_::OPTIONAL_) {
								//
							}
							else {
								std::cout << "failed\n";
								return false;
							}
						}
					}
					else {
						_stack.back().first.it_empty_prefix_vec.push_back(it_count);
					}
				}

				_stack.back().first.idx_it++;
				_stack.back().first.idx++;
			}
			else {
				bool optional = false;
				const wiz::load_data::UserType* nestedSchemaUT = nullptr;
				const wiz::load_data::UserType* nestedJsonUT = nullptr;


				if ("$id"sv == x->GetUserTypeList(ut_count)->GetName()) {
					for (size_t i = 0; i < x->GetUserTypeList(ut_count)->GetItemListSize(); ++i) {
						idMap.insert(std::make_pair(x->GetUserTypeList(ut_count)->GetItemList(i).Get().ToString(), std::set<std::string>()));
					}
					_stack.back().first.idx_ut++;
					_stack.back().first.idx++;
					continue;
				}
				else if ("$clear_id"sv == x->GetUserTypeList(ut_count)->GetName()) {
					for (size_t i = 0; i < x->GetUserTypeList(ut_count)->GetItemListSize(); ++i) {
						idMap[x->GetUserTypeList(ut_count)->GetItemList(i).Get().ToString()].clear();
					}
					_stack.back().first.idx_ut++;
					_stack.back().first.idx++;
					continue;
				}
				else {
					style_vec.clear();
					auto opt = OptionFrom(x->GetUserTypeList(ut_count)->GetName().ToString(), mainUT, style_vec, styleMap);
					std::string key = opt.prefix;
					
					nestedSchemaUT = x->GetUserTypeList(ut_count);

					if (!key.empty()) {
						auto result = _stack.back().second.objectMap.find(key);
						if (result != _stack.back().second.objectMap.end()) {
							std::vector<std::string> name_style_vec;

							if (result->second->IsUserType()) {
								std::string name = result->second->GetName().ToString();
								if (name.empty()) {
									name = "_";
								}
								if (auto success = _Check(mainUT, enumMap, styleMap, idMap, needMap, *(x->GetUserTypeList(ut_count)), *((wiz::load_data::UserType*)(result->second)),
									_stack.back().second.real_dir + "/" + name); std::get<0>(success)) {
									// std::cout << "success\n";

									_stack.back().second.visitMap[result->second] = 1;
									
									nestedJsonUT = (wiz::load_data::UserType*)result->second;
									
									_stack.back().second.count++;
								}
								else {
									if (_stack.back().first.multiple) {
										std::cout << "failed\n";
										return false;
									}
									else {
										std::cout << "failed\n";
										return false;
									}
								}
							}
							else {
								std::cout << "failed\n";
								return false;
							}
						}
						else {
							if (opt.required == Option::Required_::OPTIONAL_) {
								optional = true;
							}
							else {
								std::cout << "failed\n";
								return false;
							}
						}
					}
					else {
						_stack.back().first.ut_empty_prefix_vec.push_back(ut_count);
					}
				}

				_stack.back().first.idx_ut++;
				_stack.back().first.idx++;

				if (!optional && nestedJsonUT && nestedSchemaUT) {
					std::string name = nestedJsonUT->GetName().ToString();
					_stack.push_back({ { nestedSchemaUT, "empty_string", mainUT, styleMap, (long long)_stack.size()},
					{ nestedJsonUT,
						name.empty() ? _stack.back().second.real_dir + "/_" : _stack.back().second.real_dir + "/" + name, mainUT, styleMap, (long long)_stack.size(), &schemaOptionMap } });
				}
			}
		}


		return true;
	}

	inline bool _Validate(const wiz::load_data::UserType& _mainUT)
	{
		wiz::load_data::UserType* jsonUT;
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

		jsonUT = wiz::load_data::UserType::Find(&mainUT, start_name).second[0];
		schemaUT = wiz::load_data::UserType::Find(&mainUT, schema_name).second[0];


		// for log?
		bool log_on = false;
		const bool chk = Check(&mainUT, schemaUT, jsonUT, 0, log_on);

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
	wiz::load_data::UserType fileUT;
	std::string fileName; // to save

	try {
		// -v : version? - to do
		// -V : Validate.
		// -M : Make schema. // removal?
		if (argc == 3 && 0 == strcmp("-V", argv[1])) {
			option = "-V";

			auto chk_file = Lint::LoadFile(argv[2]);

			if (chk_file.first) {
				fileUT = std::move(chk_file.second);
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

		if (option == "-V") {
			int a = clock();
			auto chk = Lint::Validate(fileUT);
			int b = clock();
			std::cout << b - a << "ms\n";
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


