/*
 * Copyright (c) 2010 Andrea Sacco
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Andrea Sacco <andrea.sacco85@gmail.com>
 */

#include "my-simple-energy-model.h"

#include "ns3/energy-source.h"

#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/trace-source-accessor.h"

namespace ns3
{
namespace lorawan
{

NS_LOG_COMPONENT_DEFINE("MySimpleEnergyModel");
NS_OBJECT_ENSURE_REGISTERED(MySimpleEnergyModel);

TypeId
MySimpleEnergyModel::GetTypeId()
{
    static TypeId tid = TypeId("ns3::lorawan::MySimpleEnergyModel")
                            .SetParent<energy::DeviceEnergyModel>()
                            .SetGroupName("Energy")
                            .AddConstructor<MySimpleEnergyModel>()
                            .AddTraceSource("TotalEnergyConsumption",
                                            "Total energy consumption of the radio device.",
                                            MakeTraceSourceAccessor(
                                                &MySimpleEnergyModel::m_totalEnergyConsumption),
                                            "ns3::TracedValueCallback::Double");
    return tid;
}

MySimpleEnergyModel::MySimpleEnergyModel()
{
    NS_LOG_FUNCTION(this);
    m_lastUpdateTime = Seconds(0);
    m_actualCurrentA = 0.0;
    m_source = nullptr;
}

MySimpleEnergyModel::~MySimpleEnergyModel()
{
    NS_LOG_FUNCTION(this);
}

void
MySimpleEnergyModel::SetEnergySource(Ptr<energy::EnergySource> source)
{
    NS_LOG_FUNCTION(this << source);
    NS_ASSERT(source);
    m_source = source;
}

void
MySimpleEnergyModel::SetNode(Ptr<Node> node)
{
    NS_LOG_FUNCTION(this << node);
    NS_ASSERT(node);
    m_node = node;
}

Ptr<Node>
MySimpleEnergyModel::GetNode() const
{
    NS_LOG_FUNCTION(this);
    return m_node;
}

double
MySimpleEnergyModel::GetTotalEnergyConsumption() const
{
    NS_LOG_FUNCTION(this);
    Time duration = Simulator::Now() - m_lastUpdateTime;

    double energyToDecrease = 0.0;
    double supplyVoltage = m_source->GetSupplyVoltage();
    energyToDecrease = duration.GetSeconds() * m_actualCurrentA * supplyVoltage;

    m_source->UpdateEnergySource();

    return m_totalEnergyConsumption + energyToDecrease;
}

void
MySimpleEnergyModel::SetCurrentA(double current)
{
    NS_LOG_FUNCTION(this << current);
    Time duration = Simulator::Now() - m_lastUpdateTime;

    double energyToDecrease = 0.0;
    double supplyVoltage = m_source->GetSupplyVoltage();
    energyToDecrease = duration.GetSeconds() * m_actualCurrentA * supplyVoltage;

    // update total energy consumption
    m_totalEnergyConsumption += energyToDecrease;
    // update last update time stamp
    m_lastUpdateTime = Simulator::Now();

    // notify energy source
    m_source->UpdateEnergySource();

    // update the current drain
    m_actualCurrentA = current;

}

void
MySimpleEnergyModel::DoDispose()
{
    NS_LOG_FUNCTION(this);
    m_source = nullptr;
}

double
MySimpleEnergyModel::DoGetCurrentA() const
{
    NS_LOG_FUNCTION(this);
    return m_actualCurrentA;
}

} // namespace lorawan
} // namespace ns3
