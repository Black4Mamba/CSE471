#include "StdAfx.h"
#include "Chorus.h"
#include <cmath>


CChorus::CChorus(void)
{
	m_delay = 0;
	m_dry = 0;
	m_wet = 0;
	m_modulationRange = 0;
	m_modulationRate = 0;
	m_wrloc = 0;
	m_sampleRate = 0;

	m_queueL.resize(200000);
	m_queueR.resize(200000);
}


CChorus::~CChorus(void)
{
}

void CChorus::Process(double *frameIn, double *frameOut)
{
	double delayVariance = (m_modulationRange * m_delay) * sin(2 * PI * m_modulationRate);
	double newDelay = m_delay + delayVariance;

	m_wrloc = (m_wrloc + 1) % 200000;
	m_queueL[m_wrloc] = frameIn[0];
	m_queueR[m_wrloc] = frameIn[1];

	int delayLength = int((newDelay * m_sampleRate + 0.5)) * 2;
	int rdloc = (m_wrloc + 200000 - delayLength) % 200000;

	// Wet
	frameOut[0] = frameIn[0] / 2 + m_queueL[rdloc] / 2;
	frameOut[0] *= m_wet;
	frameOut[1] = frameIn[1] / 2 + m_queueR[rdloc] / 2;
	frameOut[1] *= m_wet;

	// Dry
	frameOut[0] += frameIn[0] * m_dry;
	frameOut[1] += frameIn[1] * m_dry;
}

void CChorus::XmlLoad(IXMLDOMNode * xml)
{
	// Get a list of all attribute nodes and the
	// length of that list
	CComPtr<IXMLDOMNamedNodeMap> attributes;
	xml->get_attributes(&attributes);
	long len;
	attributes->get_length(&len);

	// Loop over the list of attributes
	for (int i = 0; i<len; i++)
	{
		// Get attribute i
		CComPtr<IXMLDOMNode> attrib;
		attributes->get_item(i, &attrib);

		// Get the name of the attribute
		CComBSTR name;
		attrib->get_nodeName(&name);

		// Get the value of the attribute.  
		CComVariant value;
		attrib->get_nodeValue(&value);

		if (name == "delay")
		{
			value.ChangeType(VT_R8);
			m_delay = value.dblVal;
		}
		else if (name == "wet")
		{
			value.ChangeType(VT_R8);
			m_wet = value.dblVal;
		}
		else if (name == "dry")
		{
			value.ChangeType(VT_R8);
			m_dry = value.dblVal;
		}
		else if (name == "range")
		{
			value.ChangeType(VT_R8);
			m_modulationRange = value.dblVal;
		}
		else if (name == "rate")
		{
			value.ChangeType(VT_R8);
			m_modulationRate = value.dblVal;
		}
	}
}
