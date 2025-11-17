export module vulkan_lib.result;

import <string>;
import <source_location>;
import <expected>;
export struct EmptyOk {};

/// empty struct-type when want to return no value in
/// result error. ex: Result<Empty,EmptyErr>
export struct EmptyErr {};

export struct Error {
	std::string message;
	uint32_t code;
	std::source_location where = std::source_location::current();
};

export template<typename Ok, typename Err = EmptyErr>
using Result = std::expected<Ok, Err>;

export [[nodiscard]] inline auto
make_error(std::string message, uint32_t code = 0, std::source_location location = std::source_location::current()) -> Error {
	return Error{ std::move(message), code, location };
}

export [[nodiscard]] inline auto
error(std::string message, uint32_t code = 0, std::source_location location = std::source_location::current()) -> std::unexpected<Error> {
	return std::unexpected(Error{ std::move(message), code, location });
}
