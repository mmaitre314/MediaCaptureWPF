#pragma once

template<typename T>
class SampleCallback
{
public:
    SampleCallback(T^ parent)
        : m_parent(parent)
    {

    }

    void operator()(_In_ IMFSample* sample)
    {
        m_parent->OnSample(sample);
    }

private:
    msclr::gcroot<T^> m_parent;
};
