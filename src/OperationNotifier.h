//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Logger like singleton replaces earlier used exceptions. User of library
// can set callback functions to get information about errors or warnings.

#ifndef OPERATION_NOTIFIER_H_
#define OPERATION_NOTIFIER_H_

#include "PathBuilder.h"

#include <string>

// Defines for easier use
#define throwError eyegui::OperationNotifier::notifyAboutError
#define throwWarning eyegui::OperationNotifier::notifyAboutWarning

namespace eyegui
{
    class OperationNotifier
    {
    public:

        // Enumeration of operations which could occur
        enum class Operation { PARSING, IMAGE_LOADING, FONT_LOADING, RUNTIME, BUG };

        // Send error to callback
        static void notifyAboutError(Operation operation, std::string message = "", std::string filepath = "")
        {
            getInstance()->callbackError(operation, message, filepath);
        }

        // Send warning to callback
        static void notifyAboutWarning(Operation operation, std::string message = "", std::string filepath = "")
        {
            getInstance()->callbackWarning(operation, message, filepath);
        }

        // Set function to call back
        static void setErrorCallback(void(*pCallbackFunction)(std::string))
        {
            getInstance()->internalSetErrorCallback(pCallbackFunction);
        }

        // Set function to call back
        static void setWarningCallback(void(*pCallbackFunction)(std::string))
        {
            getInstance()->internalSetWarningCallback(pCallbackFunction);
        }

    private:

        // Enumeration of types which could occur
        enum class Type { ERROR_CALLBACK, WARNING_CALLBACK };

        // Private constructor
        OperationNotifier() { mErrorCallbackSet = false; mWarningCallbackSet = false; }

        // Destructor
        virtual ~OperationNotifier() {}

        // Private copy constuctor
        OperationNotifier(OperationNotifier const&) {}

        // Private assignment operator
        OperationNotifier& operator = (OperationNotifier const&) { return *this; }

        // Check for instance existence
        static OperationNotifier* getInstance()
        {
            if (pInstance == NULL)
            {
                static OperationNotifier operationNotifier;
                pInstance = &operationNotifier;
            }
            return pInstance;
        }

        // Set callback function
        void internalSetErrorCallback(void(*pCallbackFunction)(std::string))
        {
            mpErrorCallbackFunction = pCallbackFunction;
            mErrorCallbackSet = true;
        }

        // Set callback function
        void internalSetWarningCallback(void(*pCallbackFunction)(std::string))
        {
            mpWarningCallbackFunction = pCallbackFunction;
            mWarningCallbackSet = true;
        }

        // Notify about error
        void callbackError(Operation operation, std::string message, std::string filepath)
        {
            // Only do something when callback is given
            if (mErrorCallbackSet)
            {
                // Call callback function
                mpErrorCallbackFunction(buildContent(Type::ERROR_CALLBACK, operation, message, filepath));
            }
        }

        // Notify about warning
        void callbackWarning(Operation operation, std::string message, std::string filepath)
        {
            // Only do something when callback is given
            if (mWarningCallbackSet)
            {
                // Call callback function
                mpWarningCallbackFunction(buildContent(Type::WARNING_CALLBACK, operation, message, filepath));
            }
        }

        // Build content of callback
        std::string buildContent(Type type, Operation operation, std::string message, std::string filepath)
        {
            // Type
            std::string typeString;
            switch (type)
            {
            case Type::ERROR_CALLBACK:
                typeString = "Error";
                break;
            case Type::WARNING_CALLBACK:
                typeString = "Warning";
                break;
            }

            // Decide about operation
            std::string operationString;
            switch (operation)
            {
            case Operation::PARSING:
                operationString = "Parsing " + typeString + ": ";
                break;
            case Operation::IMAGE_LOADING:
                operationString = "Image Loading " + typeString + ": ";
                break;
            case Operation::FONT_LOADING:
                operationString = "Font Loading " + typeString + ": ";
                break;
            case Operation::RUNTIME:
                operationString = "Runtime " + typeString + ": ";
                break;
            }

            // Decide about file
            std::string fullFilepathString = "";
            if (filepath != "")
            {
                fullFilepathString = " (" + buildPath(filepath) + ")";
            }

            // Build content
            return operationString + message + fullFilepathString;
        }

        // The pointer to the single instance
        static OperationNotifier* pInstance;

        // Member
        bool mErrorCallbackSet;
        bool mWarningCallbackSet;
        void(*mpErrorCallbackFunction)(std::string);
        void(*mpWarningCallbackFunction)(std::string);
    };
}

#endif // OPERATION_NOTIFIER_H_
