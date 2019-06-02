/*
 * Service.hpp
 *
 *  Created on: Jun 1, 2019
 *      Author: chorm
 */

#ifndef INCLUDE_SERVICE_SERVICE_HPP__2019_06_01_05_47_03
#define INCLUDE_SERVICE_SERVICE_HPP__2019_06_01_05_47_03

#include <service/Service.h>
#include <smi/Invoke.h>

#include <lclib/UUID.hpp>
#include <lclib/Version.hpp>
#include <lclib/JTime.hpp>
#include <lclib/TypeTraits.hpp>

#include <string>
#include <chrono>
#include <memory>

#include <exception>

#include <type_traits>

namespace lcsrv{

	struct service_error:std::runtime_error{
		service_error(Service* srv);
	};

	struct length_mismatch_error:std::logic_error{
		length_mismatch_error(const char*,std::uint16_t,std::uint16_t);
	};
	struct type_mismatch_error:std::logic_error{
		type_mismatch_error(const char*,std::uint16_t,Type,Type);
	};
	template<typename T,typename K=void> struct encode_to_smibuff{
		static_assert(!(std::is_class_v<T>)&&(std::is_base_of_v<T,T>),"Cannot use encode_to_smibuff without specializing");
	};

	namespace _detail{
		template<typename T> struct count_helper:auto_constant<0u>{};
		template<typename T> struct count_helper<T&>:auto_constant<1u>{};

		template<std::uint16_t i> constexpr std::uint16_t constexpr_sum(){
			return i;
		}
		template<std::uint16_t i,std::uint16_t j> constexpr std::uint16_t constexpr_sum(){
			return i+j;
		}
		template<std::uint16_t i,std::uint16_t j,std::uint16_t... rest> constexpr std::uint16_t constexpr_sum(){
			return constexpr_sum<i,j>()+constexpr_sum<rest...>();
		}
		template<typename... Ts> struct return_count:auto_constant<constexpr_sum<1,count_helper<Ts>::value...>()>{};
	}


	template<> struct encode_to_smibuff<uint8_t>{
		static constexpr auto write_fn= smibuff_write_byte;
		static constexpr auto read_fn = smibuff_read_byte;
		static constexpr Type type = Byte;
	};
	template<> struct encode_to_smibuff<int8_t>{
		static constexpr auto write_fn = smibuff_write_sbyte;
		static constexpr auto read_fn = smibuff_read_sbyte;
		static constexpr Type type = SByte;
	};
	template<> struct encode_to_smibuff<int16_t>{
		static constexpr auto write_fn = smibuff_write_short;
		static constexpr auto read_fn = smibuff_read_short;
		static constexpr Type type = Short;
	};
	template<> struct encode_to_smibuff<uint16_t>{
		static constexpr auto write_fn = smibuff_write_ushort;
		static constexpr auto read_fn = smibuff_read_ushort;
		static constexpr Type type = UShort;
	};
	template<> struct encode_to_smibuff<int32_t>{
		static constexpr auto write_fn = smibuff_write_int;
		static constexpr auto read_fn = smibuff_read_int;
		static constexpr Type type = Int;
	};
	template<> struct encode_to_smibuff<uint32_t>{
		static constexpr auto write_fn = smibuff_write_uint;
		static constexpr auto read_fn = smibuff_read_uint;
		static constexpr Type type = UInt;
	};
	template<> struct encode_to_smibuff<int64_t>{
		static constexpr auto write_fn = smibuff_write_long;
		static constexpr auto read_fn = smibuff_read_long;
		static constexpr Type type = Long;
	};
	template<> struct encode_to_smibuff<float>{
		static constexpr auto write_fn = smibuff_write_float;
		static constexpr auto read_fn = smibuff_read_float;
		static constexpr Type type = Float;
	};
	template<> struct encode_to_smibuff<double>{
		static constexpr auto write_fn = smibuff_write_uint;
		static constexpr auto read_fn = smibuff_read_uint;
		static constexpr Type type = Double;
	};
	template<> struct encode_to_smibuff<version>{
		static constexpr auto write_fn = smibuff_write_version;
		static constexpr auto read_fn = smibuff_read_version;
		static constexpr Type type = VERSION;
	};
	template<> struct encode_to_smibuff<uuid>{
		static constexpr auto write_fn = smibuff_write_uuid;
		static constexpr auto read_fn = smibuff_read_uuid;
		static constexpr Type type = Uuid;
	};
	template<> struct encode_to_smibuff<instant>{
		static constexpr auto write_fn = smibuff_write_instant;
		static constexpr auto read_fn = smibuff_read_instant;
		static constexpr Type type = INSTANT;
	};
	template<> struct encode_to_smibuff<duration>{
		static constexpr auto write_fn = smibuff_write_duration;
		static constexpr auto read_fn = smibuff_read_duration;
		static constexpr Type type = DURATION;
	};
	template<> struct encode_to_smibuff<std::string>{
		static constexpr auto write_fn = [](SMIBuffer* buff,const std::string& str){smibuff_write_string(buff,str.c_str());};
		static constexpr auto read_fn = [](SMIBuffer* buff){
			std::unique_ptr<char[]> raw_str = smibuff_read_string2(buff,[](auto sz){return (void*)new char[sz];},[](void* obj){return delete[] (char*)obj;});
			return std::string{raw_str.get()};
		};
		static constexpr auto type = String;
	};
	template<> struct encode_to_smibuff<UUID>{
		static constexpr auto write_fn = [](SMIBuffer* buff,UUID id){
			uuid lid{id.getHigh(),id.getLow()};
			smibuff_write_uuid(buff,lid);
		};
		static constexpr auto read_fn = [](SMIBuffer* buff){
			uuid lid = smibuff_read_uuid(buff);
			return UUID{lid.hi,lid.lo};
		};
		static constexpr auto type = Uuid;
	};
	template<> struct encode_to_smibuff<Version>{
		static constexpr auto write_fn = [](SMIBuffer* buff,Version v){
			version lv{((uint8_t)v.getMajor()-1),((uint8_t)v.getMinor())};
			smibuff_write_version(buff,lv);
		};
		static constexpr auto read_fn = [](SMIBuffer* buff){
			version lv = smibuff_read_version(buff);
			return Version(((uint16_t)lv.maj)<<8+lv.min);
		};
		static constexpr auto type = VERSION;
	};
	template<> struct encode_to_smibuff<Instant>{
		static constexpr auto write_fn = [](SMIBuffer* buff,Instant v){
			instant i{v.toEpochSecond(),v.getNanos()};
			smibuff_write_instant(buff,i);
		};
		static constexpr auto read_fn = [](SMIBuffer* buff){
			instant i = smibuff_read_instant(buff);
			return Instant{i.seconds,i.nanos};
		};
		static constexpr auto type = INSTANT;
	};
	template<> struct encode_to_smibuff<Duration>{
		static constexpr auto write_fn = [](SMIBuffer* buff,Duration v){
			duration i{v.getSeconds(),v.getNanos()};
			smibuff_write_duration(buff,i);
		};
		static constexpr auto read_fn = [](SMIBuffer* buff){
			duration i = smibuff_read_duration(buff);
			return Duration{i.seconds,i.nanos};
		};
		static constexpr auto type = INSTANT;
	};

	template<typename E> struct encode_to_smibuff<E,std::enable_if_t<std::is_enum_v<E>>>{
		using underlying_type = std::underlying_type_t<E>;
		using _underlying_smibuff_helper = encode_to_smibuff<underlying_type>;
		static constexpr auto type = _underlying_smibuff_helper::type;
		static constexpr auto write_fn = [](SMIBuffer* buff,E e){
			_underlying_smibuff_helper::write_fn(buff,static_cast<underlying_type>(e));
		};
		static constexpr auto read_fn = [](SMIBuffer* buff){
			return static_cast<E>(_underlying_smibuff_helper::read_fn(buff));
		};
	};

	struct ServiceWrapper;

	template<typename Fn> struct MethodWrapper;

	template<typename R,typename... Args> struct MethodWrapper<R(Args...)>{
	private:
		Method* info;
	public:
		static constexpr std::uint16_t param_count = sizeof...(Args);
		static constexpr std::uint16_t return_count = _detail::return_count<Args...>::value;
		MethodWrapper(Method* m):info{m}{
			std::uint16_t expected_count = method_get_parameter_count(m);
			std::uint16_t expected_return = method_get_return_count(m);
			if(expected_count!=param_count)
				throw new length_mismatch_error("Parameter",param_count,expected_count);
		}
	};

}


#endif /* INCLUDE_SERVICE_SERVICE_HPP__2019_06_01_05_47_03 */
