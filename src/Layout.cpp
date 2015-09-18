//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE 
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Layout.h"

#include "GUI.h"
#include "Helper.h"
#include "Elements/ElementCasting.h"
#include "Defines.h"
#include "OperationNotifier.h"

namespace eyegui
{
	Layout::Layout(GUI const * pGUI, std::string stylesheetFilepath)
	{
		// Initialize members
		mpGUI = pGUI;
		mupRoot = NULL;
		mupIds = NULL;
		mAlpha = 1;
		mVisible = true;
		mResizeNecessary = true;
		mUseInput = true;
		mpSelectedInteractiveElement = NULL;
		mupNotificatons = std::unique_ptr<std::vector<std::pair<InteractiveElement*, InteractiveElement::Notification> > >(new std::vector<std::pair<InteractiveElement*, InteractiveElement::Notification> >);

		// Parse style file
		mStyles = mStylesheetParser.parse(stylesheetFilepath);
	}

	Layout::~Layout()
	{
		// Nothing to do so far
	}

	void Layout::update(float tpf, Input* pInput)
	{
		// *** NOTIFICATIONS ***

		// Repeat working on the notifications
		int loopCount = 0;
		while (mupNotificatons->size() > 0)
		{
			if (loopCount == NOTIFICATION_MAX_LOOP_COUNT)
			{
				// Delete still existing notifications
				mupNotificatons.reset(new std::vector<std::pair<InteractiveElement*, InteractiveElement::Notification> >);
				throwWarning(OperationNotifier::Operation::RUNTIME, "Notifications are too nested and were cleared");

				// Break the while loop
				break;
			}

			else
			{
				// Transfer notifications to local pointer and replace with empty one
				std::unique_ptr<std::vector<std::pair<InteractiveElement*, InteractiveElement::Notification> > > upNotifications = std::move(mupNotificatons);
				mupNotificatons = std::unique_ptr<std::vector<std::pair<InteractiveElement*, InteractiveElement::Notification> > >(new std::vector<std::pair<InteractiveElement*, InteractiveElement::Notification> >);

				// Work on notifications
				for (int i = 0; i < upNotifications->size(); i++)
				{
					// Listeners of notifications could fill the notifications in this layout again... (just a note)
					std::pair<InteractiveElement*, InteractiveElement::Notification> notification = upNotifications->at(i);

					// Piping takes care that replaced elements do not send notifications
					notification.first->pipeNotification(notification.second);
				}

				// count loops
				loopCount++;
			}
		}

		// *** OWN UPDATE ***

		// Update alpha
		if (mVisible)
		{
			mAlpha += tpf / getConfig()->animationDuration;
		}
		else
		{
			mAlpha -= tpf / getConfig()->animationDuration;
		}
		mAlpha = clamp(mAlpha, 0, 1);

		// *** UPDATE ELEMENTS ***

		// Update root only if own alpha greater zero
		if (mAlpha > 0)
		{
			// Do not use input if still fading
			if (!mUseInput || mAlpha < 1)
			{
				pInput = NULL;
			}

			// Update front elements (other way than expected because inner ones are added first)
			for (int i = 0; i < mFrontElements.size(); i++)
			{
				// Alpha value final, do not multiply with mAlpha
				Element* pElement = mFrontElements[i];
				pElement->update(tpf, mFrontElementAlphas[pElement], pInput);
			}

			// Update standard elements
			mupRoot->update(tpf, mAlpha, pInput);
		}

		// *** DELETION OF REPLACED ELEMENTS ***

		// Delete replaced elements which has been only kept alive for notifications for one frame
		mDyingReplacedElements.clear();
	}

	void Layout::draw() const
	{
		// Use alpha because while fading it should still draw
		if (mAlpha > 0)
		{
			// Draw standard elements
			mupRoot->draw();

			// Draw front elements (other way than expected because inner ones are added first)
			for (int i = (int)mFrontElements.size() - 1; i >= 0; i--)
			{
				Element const * pElement = mFrontElements[i];
				pElement->draw();
			}
		}
	}

	void Layout::resize(bool force)
	{
		if (mAlpha > 0 || force)
		{
			// Show attached root centered
			int width, height;
			mupRoot->evaluateSize(getLayoutWidth(), getLayoutHeight(), width, height);
			int deltaX = (getLayoutWidth() - width) / 2;
			int deltaY = (getLayoutHeight() - height) / 2;
			mupRoot->transformAndSize(deltaX, deltaY, width, height);
			mResizeNecessary = false;
		}
		else
		{
			mResizeNecessary = true;
		}
	}

	void Layout::attachRoot(
		std::unique_ptr<Element> upRoot,
		std::unique_ptr<std::map<std::string, Element*> > upIds)
	{
		mupRoot = std::move(upRoot);
		mupIds = std::move(upIds);
	}

	void Layout::commitDyingReplacedElement(std::unique_ptr<Element> upElement)
	{
		mDyingReplacedElements.push_back(std::move(upElement));
	}

	void Layout::enqueueNotification(InteractiveElement* pNotifier, InteractiveElement::Notification notification)
	{
		mupNotificatons->push_back(std::pair<InteractiveElement*, InteractiveElement::Notification>(pNotifier, notification));
	}

	void Layout::registerFrontElementForUpdateAndDraw(Element* pElement, bool visible)
	{
		mFrontElements.push_back(pElement);

		if (visible)
		{
			mFrontElementAlphas[pElement] = 1;
		}
		else
		{
			mFrontElementAlphas[pElement] = 0;
		}
	}

	void Layout::setFrontElementAlpha(Element* pElement, float alpha)
	{
		mFrontElementAlphas[pElement] = alpha;
	}

	Config const * Layout::getConfig() const
	{
		return mpGUI->getConfig();
	}

	uint Layout::getLayoutWidth() const
	{
		return mpGUI->getWindowWidth();
	}

	uint Layout::getLayoutHeight() const
	{
		return mpGUI->getWindowHeight();
	}

	float Layout::getAccPeriodicTime() const
	{
		return mpGUI->getAccPeriodicTime();
	}

	Style const * Layout::getStyleFromStylesheet(std::string styleName) const
	{
		// Should always work, because everything is checked at parsing
		return &((*(mStyles.get())).at(styleName));
	}

	std::set<std::string> Layout::getNamesOfAvailableStyles() const
	{
		std::set<std::string> names;
		for (const std::pair<std::string, Style>& pair : *(mStyles.get()))
		{
			names.insert(pair.first);
		}

		return names;
	}

	void Layout::setVisibility(bool visible, bool setImmediately)
	{
		mVisible = visible;

		// If visible now and resize is necessary, resize!
		if (mVisible && mResizeNecessary)
		{
			resize(true);
		}

		if (setImmediately)
		{
			if (mVisible)
			{
				mAlpha = 1;
			}
			else
			{
				mAlpha = 0;
			}
		}
	}

	void Layout::useInput(bool useInput)
	{
		mUseInput = useInput;
	}

	void Layout::setElementActivity(std::string id, bool active, bool setImmediately)
	{
		Element* pElement = fetchElement(id);
		if (pElement != NULL)
		{
			pElement->setActivity(active, setImmediately);

			if (!active && mpSelectedInteractiveElement == pElement)
			{
				// If selected element goes inactive, deselect it
				deselectInteractiveElement();
			}
		}
		else
		{
			throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find element with id: " + id);
		}
	}

	bool Layout::isElementActive(std::string id) const
	{
		Element* pElement = fetchElement(id);
		if (pElement != NULL)
		{
			return pElement->isActive();
		}
		else
		{
			throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find element with id: " + id);
		}
		return false;
	}

	void Layout::highlightInteractiveElement(std::string id, bool doHighlight)
	{
		InteractiveElement* pInteractiveElement = toInteractiveElement(fetchElement(id));
		if (pInteractiveElement != NULL)
		{
			pInteractiveElement->highlight(doHighlight);
		}
		else
		{
			throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find interactive element with id: " + id);
		}
	}

	void Layout::setIconOfInteractiveElement(std::string id, std::string iconFilepath)
	{
		InteractiveElement* pInteractiveElement = toInteractiveElement(fetchElement(id));
		if (pInteractiveElement != NULL)
		{
			pInteractiveElement->setIcon(iconFilepath);
		}
		else
		{
			throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find interactive element with id: " + id);
		}
	}

	void Layout::hitButton(std::string id)
	{
		Button* pButton = toButton(fetchElement(id));
		if (pButton != NULL)
		{
			return pButton->hit();
		}
		else
		{
			throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find button with id: " + id);
		}
	}

	void Layout::buttonDown(std::string id)
	{
		Button* pButton = toButton(fetchElement(id));
		if (pButton != NULL)
		{
			return pButton->down();
		}
		else
		{
			throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find button with id: " + id);
		}
	}

	void Layout::buttonUp(std::string id)
	{
		Button* pButton = toButton(fetchElement(id));
		if (pButton != NULL)
		{
			return pButton->up();
		}
		else
		{
			throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find button with id: " + id);
		}
	}

	bool Layout::isButtonSwitch(std::string id) const
	{
		Button* pButton = toButton(fetchElement(id));
		if (pButton != NULL)
		{
			return pButton->isSwitch();
		}
		else
		{
			throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find button with id: " + id);
		}
		return false;
	}

	void Layout::penetrateSensor(std::string id, float amount)
	{
		Sensor* pSensor = toSensor(fetchElement(id));
		if (pSensor != NULL)
		{
			pSensor->penetrate(amount);
		}
		else
		{
			throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find sensor with id: " + id);
		}
	}

	void Layout::registerButtonListener(std::string id, std::weak_ptr<ButtonListener> wpListener)
	{
		Button* pButton = toButton(fetchElement(id));
		if (pButton != NULL)
		{
			pButton->registerListener(wpListener);
		}
		else
		{
			throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find button with id: " + id);
		}
	}

	void Layout::registerSensorListener(std::string id, std::weak_ptr<SensorListener> wpListener)
	{
		Sensor* pSensor = toSensor(fetchElement(id));
		if (pSensor != NULL)
		{
			pSensor->registerListener(wpListener);
		}
		else
		{
			throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find sensor with id: " + id);
		}
	}

	void Layout::selectInteractiveElement(std::string id)
	{
		InteractiveElement* pInteractiveElement = toInteractiveElement(fetchElement(id));
		if (pInteractiveElement != NULL)
		{
			selectInteractiveElement(pInteractiveElement);
		}
		else
		{
			throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find interactive element with id: " + id);
		}
	}

	bool Layout::selectInteractiveElement(InteractiveElement* pInteractiveElement)
	{
		// Deselect currently selected interactive element
		deselectInteractiveElement();

		// Only select next, if visible
		if (mAlpha >= 1 && pInteractiveElement->getAlpha() > 0)
		{
			mpSelectedInteractiveElement = pInteractiveElement;
			mpSelectedInteractiveElement->select(true);
			return true;
		}
		else
		{
			return false;
		}
	}

	void Layout::deselectInteractiveElement()
	{
		if (mpSelectedInteractiveElement != NULL)
		{
			mpSelectedInteractiveElement->select(false);
			mpSelectedInteractiveElement = NULL;
		}
	}

	void Layout::interactWithSelectedInteractiveElement()
	{
		if (mpSelectedInteractiveElement != NULL && mAlpha >= 1)
		{
			mpSelectedInteractiveElement->interact();
		}
	}

	bool Layout::selectNextInteractiveElement()
	{
		// Take a guess
		bool reachedEndOfLayout = true;

		// Pointer to next interactive element
		InteractiveElement* pNext;

		if (mpSelectedInteractiveElement == NULL)
		{
			// Nothing selected at the moment, so start at root
			pNext = mupRoot->nextInteractiveElement();
		}
		else
		{
			// Try to get next interactive element
			pNext = mpSelectedInteractiveElement->nextInteractiveElement();
		}

		// Only do something, if something was found
		if (pNext != NULL)
		{
			if (selectInteractiveElement(pNext))
			{
				reachedEndOfLayout = false;
			}
			else
			{
				// Some problem occurent selecting the next element
				deselectInteractiveElement();
			}
		}
		else
		{
			// No next element found
			deselectInteractiveElement();
		}

		return reachedEndOfLayout;
	}

	void Layout::reset()
	{
		mupRoot->reset();
	}

	bool Layout::checkForId(std::string id) const
	{
		Element const* pElement = fetchElement(id);
		if (pElement == NULL)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	bool Layout::isInteractiveElementHighlighted(std::string id) const
	{
		InteractiveElement* pInteractiveElement = toInteractiveElement(fetchElement(id));
		if (pInteractiveElement != NULL)
		{
			return pInteractiveElement->isHighlighted();
		}
		else
		{
			throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find interactive element with id: " + id);
		}
		return false;
	}

	void Layout::replaceElementWithBlock(std::string id, bool doFading)
	{
		Element* pElement = fetchElement(id);
		if (pElement != NULL)
		{
			// Create new block
			std::unique_ptr<Block> upBlock = std::unique_ptr<Block>(new Block(
				pElement->getId(),
				pElement->getStyleName(),
				pElement->getParent(),
				pElement->getLayout(),
				pElement->getAssetManager(),
				pElement->getRelativeScale(),
				pElement->getBorder()));

			Element* pBlock = upBlock.get();

			// Replace target with it
			if (replaceElement(pElement, std::move(upBlock), doFading))
			{
				insertId(pBlock);
			}
		}
		else
		{
			throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find element with id: " + id);
		}
	}

	void Layout::replaceElementWithPicture(std::string id, bool doFading, std::string filepath, PictureAlignment alignment)
	{
		Element* pElement = fetchElement(id);
		if (pElement != NULL)
		{
			// Create new picture
			std::unique_ptr<Picture> upPicture = std::unique_ptr<Picture>(new Picture(
				pElement->getId(),
				pElement->getStyleName(),
				pElement->getParent(),
				pElement->getLayout(),
				pElement->getAssetManager(),
				pElement->getRelativeScale(),
				pElement->getBorder(),
				filepath,
				alignment));

			Element* pPicture = upPicture.get();

			// Replace target with it
			if (replaceElement(pElement, std::move(upPicture), doFading))
			{
				insertId(pPicture);
			}
		}
		else
		{
			throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find element with id: " + id);
		}
	}

	void Layout::replaceElementWithBlank(std::string id, bool doFading)
	{
		Element* pElement = fetchElement(id);
		if (pElement != NULL)
		{
			// Create new blank
			std::unique_ptr<Blank> upBlank = std::unique_ptr<Blank>(new Blank(
				pElement->getId(),
				pElement->getStyleName(),
				pElement->getParent(),
				pElement->getLayout(),
				pElement->getAssetManager(),
				pElement->getRelativeScale(),
				pElement->getBorder()));

			Element* pBlank = upBlank.get();

			// Replace target with it
			if (replaceElement(pElement, std::move(upBlank), doFading))
			{
				insertId(pBlank);
			}
		}
		else
		{
			throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find element with id: " + id);
		}
	}

	void Layout::replaceElementWithCircleButton(std::string id, bool doFading, std::string iconFilepath, bool isSwitch)
	{
		Element* pElement = fetchElement(id);
		if (pElement != NULL)
		{
			// Create new picture
			std::unique_ptr<CircleButton> upCircleButton = std::unique_ptr<CircleButton>(new CircleButton(
				pElement->getId(),
				pElement->getStyleName(),
				pElement->getParent(),
				pElement->getLayout(),
				pElement->getAssetManager(),
				pElement->getRelativeScale(),
				pElement->getBorder(),
				iconFilepath,
				isSwitch));

			Element* pCircleButton = upCircleButton.get();

			// Replace target with it
			if (replaceElement(pElement, std::move(upCircleButton), doFading))
			{
				insertId(pCircleButton);
			}
		}
		else
		{
			throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find element with id: " + id);
		}
	}

	void Layout::replaceElementWitBoxButton(std::string id, bool doFading, std::string iconFilepath, bool isSwitch)
	{
		Element* pElement = fetchElement(id);
		if (pElement != NULL)
		{
			// Create new picture
			std::unique_ptr<BoxButton> upBoxButton = std::unique_ptr<BoxButton>(new BoxButton(
				pElement->getId(),
				pElement->getStyleName(),
				pElement->getParent(),
				pElement->getLayout(),
				pElement->getAssetManager(),
				pElement->getRelativeScale(),
				pElement->getBorder(),
				iconFilepath,
				isSwitch));

			Element* pBoxButton = upBoxButton.get();

			// Replace target with it
			if (replaceElement(pElement, std::move(upBoxButton), doFading))
			{
				insertId(pBoxButton);
			}
		}
		else
		{
			throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find element with id: " + id);
		}
	}

	void Layout::replaceElementWitSensor(std::string id, bool doFading, std::string iconFilepath)
	{
		Element* pElement = fetchElement(id);
		if (pElement != NULL)
		{
			// Create new picture
			std::unique_ptr<Sensor> upSensor = std::unique_ptr<Sensor>(new Sensor(
				pElement->getId(),
				pElement->getStyleName(),
				pElement->getParent(),
				pElement->getLayout(),
				pElement->getAssetManager(),
				pElement->getRelativeScale(),
				pElement->getBorder(),
				iconFilepath));

			Element* pSensor = upSensor.get();

			// Replace target with it
			if (replaceElement(pElement, std::move(upSensor), doFading))
			{
				insertId(pSensor);
			}
		}
		else
		{
			throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find element with id: " + id);
		}
	}

	void Layout::replaceElementWithBrick(std::string id, bool doFading, std::string filepath)
	{
		Element* pElement = fetchElement(id);
		if (pElement != NULL)
		{
			std::unique_ptr<elementsAndIds> upPair = std::move(mBrickParser.parse(this, pElement->getAssetManager(), pElement->getParent(), filepath));
			if (replaceElement(pElement, std::move(upPair->first), doFading))
			{
				insertIds(std::move(upPair->second));
			}
		}
		else
		{
			throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find element with id: " + id);
		}
	}

	Element* Layout::fetchElement(std::string id) const
	{
		auto it = mupIds->find(id);

		if (it != mupIds->end())
		{
			return it->second;
		}
		else
		{
			return NULL;
		}
	}

	bool Layout::replaceElement(Element* pTarget, std::unique_ptr<Element> upElement, bool doFading)
	{
		// Pointer to replacement
		Element* pElement = upElement.get();

		// Do the replacement
		std::unique_ptr<Element> upTarget;
		if (pTarget->getParent() != NULL)
		{
			// Fetch returned pointer
			upTarget = std::move(pTarget->getParent()->replaceAttachedElement(pTarget, std::move(upElement)));
		}
		else
		{
			// No parent? So must be the root
			upTarget = std::move(mupRoot);
			mupRoot = std::move(upElement);
		}

		// Check, whether element to be replaced was found and not yet replaced
		if (upTarget != NULL)
		{
			// Set activity
			pElement->setActivity(pTarget->isActive(), true);

			// Give new element replaced one for fading
			pElement->commitReplacedElement(std::move(upTarget), doFading);

			// Some reminder to handle selection later
			bool shouldSelected = false;

			// Selection
			if (mpSelectedInteractiveElement == pTarget)
			{
				shouldSelected = true;
			}

			// Get pointers to children to determine, whether child is selected
			if (!shouldSelected)
			{
				std::set<Element*> children = pTarget->getAllChildren();
				for (Element* pChild : children)
				{
					if (mpSelectedInteractiveElement == pChild)
					{
						shouldSelected = true;
						break;
					}
				}
			}

			// Do selection
			if (shouldSelected)
			{
				InteractiveElement* pInteractiveElement = toInteractiveElement(pElement);
				if (pInteractiveElement != NULL)
				{
					selectInteractiveElement(pInteractiveElement);
				}
				else
				{
					// Just deselect
					deselectInteractiveElement();
				}
			}

			// Get rid of front element
			for (Element* pFrontElement : mFrontElements)
			{
				if (pFrontElement == pTarget)
				{
					mFrontElementAlphas.erase(pTarget);
					mFrontElements.erase(std::remove(mFrontElements.begin(), mFrontElements.end(), pTarget), mFrontElements.end());
					break; // Iterator is more or less dead now, break must be
				}
			}

			// Also, kill all front element pointer to children
			std::set<Element*> children = pTarget->getAllChildren();
			for (Element* pChild : children)
			{
				for (Element* pFrontElement : mFrontElements)
				{
					if (pFrontElement == pChild)
					{
						mFrontElementAlphas.erase(pFrontElement);
						mFrontElements.erase(std::remove(mFrontElements.begin(), mFrontElements.end(), pFrontElement), mFrontElements.end());
						break; // Iterator is more or less dead now, break must be
					}
				}
			}

			// Remove target and all children from id map
			if (pTarget->getId() != EMPTY_STRING_ATTRIBUTE)
			{
				mupIds->erase(pTarget->getId());
			}
			std::set<std::string> childrenIds = pTarget->getAllChildrensIds();
			for (std::string id : childrenIds)
			{
				mupIds->erase(id);
			}

			// Do resizing of whole layout
			resize();

			// Success
			return true;
		}

		return false;
	}

	void Layout::insertId(Element* pElement)
	{
		int idCount = (int)mupIds->size();

		if (pElement->getId() != EMPTY_STRING_ATTRIBUTE)
		{
			(*(mupIds.get()))[pElement->getId()] = pElement;
		}

		if (idCount == (int)mupIds->size())
		{
			throwError(OperationNotifier::Operation::RUNTIME, "Following id is not unique: " + pElement->getId());
		}
	}

	void Layout::insertIds(std::unique_ptr<idMap> upIdMap)
	{
		int idCount = (int)(mupIds->size() + upIdMap->size());
		mupIds->insert(upIdMap->begin(), upIdMap->end());

		if (mupIds->size() != idCount)
		{
			throwError(OperationNotifier::Operation::RUNTIME, "Ids are no more unique");
		}
	}
}
