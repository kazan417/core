#include "json_values.h"

// for string <=> wstring conversion
#include "../../common/File.h"

namespace NSJSON
{
	IBaseValue::IBaseValue()
	{
	}

	IBaseValue::~IBaseValue()
	{
	}

	CPrimitive::CPrimitive(bool value) : m_type(ptBoolean)
	{
		m_bool = value;
	}

	CPrimitive::CPrimitive(int value) : m_type(ptInteger)
	{
		m_int = value;
	}

	CPrimitive::CPrimitive(double value) : m_type(ptDouble)
	{
		m_double = value;
	}

	CPrimitive::CPrimitive(const std::string& str) : m_type(ptStringA)
	{
		new (&m_string) std::string(str);
	}

	CPrimitive::CPrimitive(const std::wstring& wstr) : m_type(ptStringW)
	{
		new (&m_wstring) std::wstring(wstr);
	}

	CPrimitive::~CPrimitive()
	{
		switch (m_type)
		{
		case ptStringA:
			m_string.~basic_string<char>();
			break;
		case ptStringW:
			m_wstring.~basic_string<wchar_t>();
			break;
		default:
			break;
		}
	}

	bool CPrimitive::isBool() const
	{
		return m_type == ptBoolean;
	}

	bool CPrimitive::isInt() const
	{
		return m_type == ptInteger;
	}

	bool CPrimitive::isDouble() const
	{
		return m_type == ptDouble || m_type == ptInteger;
	}

	bool CPrimitive::isStringA() const
	{
		return m_type == ptStringA;
	}

	bool CPrimitive::isStringW() const
	{
		return m_type == ptStringW;
	}

	bool CPrimitive::toBool() const
	{
		if (m_type == ptBoolean)
			return m_bool;
#ifdef JSON_DEBUG
		throw std::bad_cast();
#endif
		return false;
	}

	int CPrimitive::toInt() const
	{
		if (m_type == ptInteger)
			return m_int;
		if (m_type == ptDouble)
			return (int)m_double;
#ifdef JSON_DEBUG
		throw std::bad_cast();
#endif
		return 0;
	}

	double CPrimitive::toDouble() const
	{
		if (m_type == ptDouble)
			return m_double;
		if (m_type == ptInteger)
			return (double)m_int;
#ifdef JSON_DEBUG
		throw std::bad_cast();
#endif
		return 0.0;
	}

	std::string CPrimitive::toStringA() const
	{
		if (m_type == ptStringA)
			return m_string;

		if (m_type == ptStringW)
		{
			return U_TO_UTF8(m_wstring);
		}

#ifdef JSON_DEBUG
		throw std::bad_cast();
#endif
		return "";
	}

	std::wstring CPrimitive::toStringW() const
	{
		if (m_type == ptStringW)
			return m_wstring;

		if (m_type == ptStringA)
		{
			return UTF8_TO_U(m_string);
		}

#ifdef JSON_DEBUG
		throw std::bad_cast();
#endif
		return L"";
	}

	CArray::CArray(int count) : m_values(count)
	{
	}

	CArray::CArray(std::initializer_list<CValue> elements) : m_values(elements)
	{
	}

	CArray::~CArray()
	{
	}

	int CArray::getCount() const
	{
		return (int)m_values.size();
	}

	CValue& CArray::get(int index)
	{
		return m_values[index];
	}

	CTypedArray::CTypedArray(BYTE* data, int len, bool isExternalize) : m_data(data), m_len(len), m_isExternalize(isExternalize)
	{
	}

	CTypedArray::~CTypedArray()
	{
		if (!m_isExternalize)
		{
			CValue::FreeTypedArray(m_data, m_len);
		}
	}

	BYTE* CTypedArray::getData()
	{
		return m_data;
	}

	int CTypedArray::getCount() const
	{
		return m_len;
	}

	CObject::CObject()
	{
	}

	CObject::~CObject()
	{
	}

	CValue& CObject::get(const std::string& name)
	{
		return m_values[name];
	}

	std::vector<std::string> CObject::getPropertyNames()
	{
		std::vector<std::string> ret;
		for (const storage_t::value_type& entry : m_values)
		{
			if (!entry.second.IsUndefined())
				ret.push_back(entry.first);
		}
		return ret;
	}

	CImage::CImage(BYTE* bits, const int& width, const int& height, const ImageFormat& format, const bool& isExternalize) :
		m_bits(bits), m_width(width), m_height(height), m_format(format), m_isExternalize(isExternalize)
	{
	}

	CImage::~CImage()
	{
		if (!m_isExternalize)
		{
			CValue::FreeImageBits(m_bits);
		}
	}

	void CImage::alloc(const int& width, const int& height, const ImageFormat& format)
	{
		if (!m_isExternalize && m_bits)
		{
			CValue::FreeImageBits(m_bits);
		}

		m_bits = CValue::AllocImageBits(width, height);
		m_width = width;
		m_height = height;
		m_format = format;
		m_isExternalize = false;
	}

	BYTE* CImage::getBits()
	{
		return m_bits;
	}

	int CImage::getWidth()
	{
		return m_width;
	}

	int CImage::getHeight()
	{
		return m_height;
	}

	ImageFormat CImage::getFormat()
	{
		return m_format;
	}

	void CImage::externalize()
	{
		m_isExternalize = true;
	}
}
