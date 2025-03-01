#include "AudioConnectionPoint.h"
#include "../Components/PluginNodeComponent.h"
#include "../GUI/PluginEditorCanvas.h"

AudioConnectionPoint::AudioConnectionPoint(PluginNodeComponent& owner, ConnectionPointType type, int index)
    : ownerNode(owner)
    , connectionType(type)
    , portIndex(index)
{
    setSize(10, 10);
}

void AudioConnectionPoint::paint(juce::Graphics& g)
{
    g.setColour(isConnected ? juce::Colours::green : juce::Colours::white);
    g.fillEllipse(getLocalBounds().toFloat());
}

void AudioConnectionPoint::mouseDown(const juce::MouseEvent& e)
{
    if (e.mods.isLeftButtonDown())
    {
        isDragging = true;
        dragStartPosition = getPosition().toFloat();
        currentDragPosition = dragStartPosition;
        
        if (auto* canvas = dynamic_cast<PluginEditorCanvas*>(getParentComponent()))
        {
            canvas->updateDragLine(dragStartPosition);
        }
    }
}

void AudioConnectionPoint::mouseDrag(const juce::MouseEvent& e)
{
    if (isDragging)
    {
        currentDragPosition = e.position + getPosition().toFloat();
        
        if (auto* canvas = dynamic_cast<PluginEditorCanvas*>(getParentComponent()))
        {
            canvas->updateDragLine(currentDragPosition);
        }
    }
}

void AudioConnectionPoint::mouseUp(const juce::MouseEvent& e)
{
    if (isDragging)
    {
        isDragging = false;
        
        if (auto* canvas = dynamic_cast<PluginEditorCanvas*>(getParentComponent()))
        {
            canvas->clearDragLine();
        }
    }
}

bool AudioConnectionPoint::isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails& details)
{
    if (auto* source = dynamic_cast<AudioConnectionPoint*>(details.sourceComponent.get()))
    {
        if (connectionType == ConnectionPointType::Input)
            return source->getType() == ConnectionPointType::Output;
        else
            return source->getType() == ConnectionPointType::Input;
    }
    return false;
}

void AudioConnectionPoint::itemDropped(const juce::DragAndDropTarget::SourceDetails& details)
{
    if (auto* source = dynamic_cast<AudioConnectionPoint*>(details.sourceComponent.get()))
    {
        if (canConnect(source))
        {
            createConnection(source);
        }
    }
}

bool AudioConnectionPoint::canConnect(AudioConnectionPoint* other) const
{
    if (!other || &other->getOwner() == &ownerNode)
        return false;
        
    return (connectionType == ConnectionPointType::Input && 
            other->getType() == ConnectionPointType::Output) ||
           (connectionType == ConnectionPointType::Output && 
            other->getType() == ConnectionPointType::Input);
}

void AudioConnectionPoint::createConnection(AudioConnectionPoint* other)
{
    if (auto* canvas = dynamic_cast<PluginEditorCanvas*>(getParentComponent()))
    {
        if (connectionType == ConnectionPointType::Input)
            canvas->addConnection(other, this);
        else
            canvas->addConnection(this, other);
    }
}

void AudioConnectionPoint::disconnect()
{
    isConnected = false;
    repaint();
}

AudioConnectionPoint* AudioConnectionPoint::findConnectionTarget(const juce::Point<int>& screenPosition) const
{
    if (auto* canvas = dynamic_cast<PluginEditorCanvas*>(getParentComponent()))
    {
        auto localPos = canvas->getLocalPoint(nullptr, screenPosition);
        
        if (auto* comp = canvas->getComponentAt(localPos.x, localPos.y))
        {
            if (auto* point = dynamic_cast<AudioConnectionPoint*>(comp))
            {
                if (canConnect(point))
                    return point;
            }
        }
    }
    return nullptr;
}

void AudioConnectionPoint::resized()
{
    repaint();
}
