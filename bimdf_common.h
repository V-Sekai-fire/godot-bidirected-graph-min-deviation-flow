#ifdef BIMDF_USE_CPP_EXCEPTIONS
#define BIMDF_WHAT what
#define BIMDF_EXCEPTION std::exception
#define BIMDF_RUNTIME_ERROR std::runtime_error
#define BIMDF_LOGIC_ERROR std::logic_error
#define BIMDF_INVALID_ARGUMENT std::invalid_argument
#define BIMDF_OUT_OF_RANGE std::out_of_range
#define BIMDF_THROW_SPECIFIER throw()

#define BIMDF_THROW throw
#define BIMDF_THROW_WITHIN(exception) throw exception
#define BIMDF_BEGIN
#define BIMDF_TRY try
#define BIMDF_CATCH catch
#define BIMDF_WRAPPED(T) T
#define BIMDF_VOID
#define BIMDF_RET return

#define BIMDF_UNWRAP_EXPR(func) (func)
#define BIMDF_CHECK_EXCEPTION
#define BIMDF_RETHROW throw

#define BIMDF_UNWRAP_CATCH(func) (func)
#define BIMDF_UNWRAP_RETHROW(func) (func)
#define BIMDF_UNWRAP_CATCH_DECL(typ, var, func) typ var = (func)
#define BIMDF_UNWRAP_RETHROW_DECL(typ, var, func) typ var = (func)
#define BIMDF_UNWRAP_CATCH_VAR(var, func) var = (func)
#define BIMDF_UNWRAP_RETHROW_VAR(var, func) var = (func)
#define BIMDF_UNWRAP_CATCH_ARG(wrap, func) wrap((func))
#define BIMDF_UNWRAP_RETHROW_ARG(wrap, func) wrap((func))
#define BIMDF_COMMA ,

#else

#define BIMDF_WHAT c_str
#define BIMDF_EXCEPTION std::string
#define BIMDF_RUNTIME_ERROR std::string
#define BIMDF_LOGIC_ERROR std::string
#define BIMDF_INVALID_ARGUMENT std::string
#define BIMDF_OUT_OF_RANGE std::string
#define BIMDF_THROW_SPECIFIER noexcept

#ifdef _MSC_VER
#pragma warning(error : 4715)
#pragma warning(error : 4716)
#pragma warning(error : 4834)
#endif

class Void {};

class [[nodiscard]] ExceptionCast {
	BIMDF_EXCEPTION &&bimdf_exception;

public:
	ExceptionCast(BIMDF_EXCEPTION &&p_bimdf_exception) :
			bimdf_exception(std::move(p_bimdf_exception)) {
	}
	BIMDF_EXCEPTION &&exception() {
		return std::move(bimdf_exception);
	}
};
template <class T>
struct [[nodiscard]] WrappedResult {
	using ExT = BIMDF_EXCEPTION;
	T bimdf_value;
	ExT bimdf_exception;
	bool bimdf_is_exception;

	WrappedResult(T &&value) :
			bimdf_value(std::move(value)), bimdf_exception(ExT()), bimdf_is_exception(false) {
	}
	WrappedResult(const T &value) :
			bimdf_value(value), bimdf_exception(ExT()), bimdf_is_exception(false) {
	}
	WrappedResult(ExceptionCast exception_cast) :
			bimdf_value(T()), bimdf_exception(std::move(exception_cast.exception())), bimdf_is_exception(true) {
	}
};

template<>
struct [[nodiscard]] WrappedResult<void> {
	using ExT = BIMDF_EXCEPTION;
	Void bimdf_value;
	ExT bimdf_exception;
	bool bimdf_is_exception;

	WrappedResult() :
			bimdf_value(Void()), bimdf_exception(ExT()), bimdf_is_exception(false) {
	}
	WrappedResult(Void) :
			bimdf_value(Void()), bimdf_exception(ExT()), bimdf_is_exception(false) {
	}
	WrappedResult(ExceptionCast exception_cast) :
			bimdf_value(Void()), bimdf_exception(std::move(exception_cast.exception())), bimdf_is_exception(true) {
	}
};

#define BIMDF_WRAPPED_DEFAULT_CONSTRUCTABLE(clsname) friend struct WrappedResult<clsname>
#define BIMDF_THROW return (::ExceptionCast)
#define BIMDF_TRY BIMDF_EXCEPTION e; bool bimdfexc_was_thrown;
#define BIMDF_BEGIN BIMDF_EXCEPTION e; bool bimdfexc_was_thrown
#define BIMDF_CATCH(extype) while (false) bimdfexc_catch_label: 
#define BIMDF_WRAPPED(T) ::WrappedResult<T>
#define BIMDF_THROW_WITHIN(exception) while ((e = (exception)), (bimdfexc_was_thrown = true)) goto bimdfexc_catch_label
#define BIMDF_VOID ::WrappedResult<void>()
#define BIMDF_RET return BIMDF_VOID

#define BIMDF_UNWRAP_EXPR(func) ([&](){ \
		auto _bimdfexc_tmp = (func); \
		e = _bimdfexc_tmp.bimdf_exception; \
		bimdfexc_was_thrown = _bimdfexc_tmp.bimdf_is_exception || bimdfexc_was_thrown; \
		return _bimdfexc_tmp.bimdf_value; \
})()
#define BIMDF_CATCH_EXCEPTION while (bimdfexc_was_thrown) goto bimdfexc_catch_label
#define BIMDF_RETHROW while (bimdfexc_was_thrown) return (::ExceptionCast)std::move(e)

#define BIMDF_UNWRAP_CATCH(func) do { BIMDF_UNWRAP_EXPR(func); BIMDF_CATCH_EXCEPTION; } while (0)
#define BIMDF_UNWRAP_RETHROW(func) do { BIMDF_UNWRAP_EXPR(func); BIMDF_RETHROW; } while (0)
#define BIMDF_UNWRAP_CATCH_ARG(wrap, func) do { wrap(BIMDF_UNWRAP_EXPR(func)); BIMDF_CATCH_EXCEPTION; } while (0)
#define BIMDF_UNWRAP_RETHROW_ARG(wrap, func) do { wrap(BIMDF_UNWRAP_EXPR(func)); BIMDF_RETHROW; } while (0)
// Note: in a single-line if statement, this would fall out.
// This should only be used for variable declarations which cannot occur in a single line.
#define BIMDF_UNWRAP_CATCH_DECL(typ, var, func) typ var = BIMDF_UNWRAP_EXPR(func); BIMDF_CATCH_EXCEPTION;
#define BIMDF_UNWRAP_RETHROW_DECL(typ, var, func) typ var = BIMDF_UNWRAP_EXPR(func); BIMDF_RETHROW;
#define BIMDF_UNWRAP_CATCH_VAR(var, func) do { var = BIMDF_UNWRAP_EXPR(func); BIMDF_CATCH_EXCEPTION; } while (0)
#define BIMDF_UNWRAP_RETHROW_VAR(var, func) do { var = BIMDF_UNWRAP_EXPR(func); BIMDF_RETHROW; } while (0)
#define BIMDF_COMMA ,

#endif