// -------------------------------------------------------------------------------------------------
/**
 *  @file cm_sim.c
 *
 *  Handle sim related functionality
 *
 *  Copyright (C) Sierra Wireless Inc. Use of this work is subject to license.
 */
// -------------------------------------------------------------------------------------------------

#include "legato.h"
#include "interfaces.h"
#include "cm_sim.h"
#include "cm_common.h"

#define CFG_MODEMSERVICE_SIM_PATH "/modemServices/sim"
#define CFG_NODE_PIN              "pin"

static uint32_t SimSlot = 1;

// -------------------------------------------------------------------------------------------------
/**
 *  Print the help text to stdout.
 */
// -------------------------------------------------------------------------------------------------
void cm_sim_PrintSimHelp
(
    void
)
{
    printf("SIM usage\n"
            "=========\n\n"
            "To get sim status:\n"
            "\tcm sim\n"
            "\tcm sim status\n\n"
            "To get sim information:\n"
            "\tcm sim info\n\n"
            "To get the sim imsi:\n"
            "\tcm sim imsi\n\n"
            "To get the sim iccid:\n"
            "\tcm sim iccid\n\n"
            "To get the sim phone number:\n"
            "\tcm sim number\n\n"
            "To enter pin code:\n"
            "\tcm sim enterpin <pin>\n\n"
            "To change pin code:\n"
            "\tcm sim changepin <oldpin> <newpin>\n\n"
            "To lock sim:\n"
            "\tcm sim lock <pin>\n\n"
            "To unlock sim:\n"
            "\tcm sim unlock <pin>\n\n"
            "To unblock sim:\n"
            "\tcm sim unblock <puk> <newpin>\n\n"
            "To store pin:\n"
            "\tcm sim storepin <pin>\n\n"
            "Enter PIN: Enters the PIN code that is required before any Mobile equipment functionality can be used.\n"
            "Change PIN: Change the PIN code of the SIM card.\n"
            "Lock: Enable security of the SIM card, it will request for a PIN code upon insertion.\n"
            "Unlock: Disable security of the SIM card, it won't request a PIN code upon insertion (unsafe).\n"
            "Unblock: Unblocks the SIM card. The SIM card is blocked after X unsuccessful attempts to enter the PIN.\n\n"
            "Whether security is enabled or not, the SIM card has a PIN code that must be entered for every operations.\n"
            "Only ways to change this PIN code are through 'changepin' and 'unblock' operations.\n\n"
            );
}

// -------------------------------------------------------------------------------------------------
/**
 *  This function returns a SIM reference, and exit application on failure.
 */
// -------------------------------------------------------------------------------------------------
static le_sim_ObjRef_t GetSimRef
(
    uint32_t simSlot
)
{
    le_sim_ObjRef_t simRef = NULL;

    if (SimSlot != 1)
    {
        printf("SIM slot: %d\n", SimSlot);
    }

    simRef = le_sim_Create(SimSlot);

    if (!simRef)
    {
        fprintf(stderr, "Invalid Slot (%u)\n", simSlot);
        exit(EXIT_FAILURE);
    }

    return simRef;
}


// -------------------------------------------------------------------------------------------------
/**
 *  This function will attempt to get the SIM state.
 *
 *  @return EXIT_SUCCESS if the call was successful, EXIT_FAILURE otherwise.
 */
// -------------------------------------------------------------------------------------------------
int cm_sim_GetSimStatus()
{
    le_sim_ObjRef_t simRef = NULL;
    le_sim_States_t state = LE_SIM_STATE_UNKNOWN;

    simRef = GetSimRef(SimSlot);

    state = le_sim_GetState(simRef);

    switch (state)
    {
        case LE_SIM_INSERTED:
            printf("SIM card is inserted and locked (LE_SIM_INSERTED).\n");
            break;
        case LE_SIM_ABSENT:
            printf("SIM card is absent (LE_SIM_ABSENT).\n");
            break;
        case LE_SIM_READY:
            printf("SIM card is inserted and unlocked (LE_SIM_READY).\n");
            break;
        case LE_SIM_BLOCKED:
            printf("SIM card is blocked (LE_SIM_BLOCKED).\n");
            break;
        case LE_SIM_BUSY:
            printf("SIM card is busy (LE_SIM_BUSY).\n");
            break;
        default:
            printf("Unknown SIM state.\n");
            break;
    }

    le_sim_Delete(simRef);
    printf("\n");

    return EXIT_SUCCESS;
}

// -------------------------------------------------------------------------------------------------
/**
*  This function will attempt to get the home network name.
*
*  @return EXIT_SUCCESS if the call was successful, EXIT_FAILURE otherwise.
*/
// -------------------------------------------------------------------------------------------------
int cm_sim_GetNetworkOperator
(
    void
)
{
    char homeNetwork[100];
    le_result_t res;
    int ret = EXIT_SUCCESS;

    res = le_sim_GetHomeNetworkOperator(homeNetwork, sizeof(homeNetwork));

    if (res != LE_OK)
    {
        homeNetwork[0] = '\0';
        ret = EXIT_FAILURE;
    }

    cm_cmn_FormatPrint("Home Network Operator", homeNetwork);

    return ret;
}

// -------------------------------------------------------------------------------------------------
/**
*  This function will attempt to get the SIM IMSI.
*
*  @return EXIT_SUCCESS if the call was successful, EXIT_FAILURE otherwise.
*/
// -------------------------------------------------------------------------------------------------
int cm_sim_GetSimImsi
(
    void
)
{
    char imsi[LE_SIM_IMSI_BYTES];
    le_result_t res;
    le_sim_ObjRef_t simRef = NULL;
    int ret = EXIT_SUCCESS;

    simRef = GetSimRef(SimSlot);

    res = le_sim_GetIMSI(simRef, imsi, sizeof(imsi));

    if (res != LE_OK)
    {
        imsi[0] = '\0';
        ret = EXIT_FAILURE;
    }

    cm_cmn_FormatPrint("IMSI", imsi);

    return ret;
}

// -------------------------------------------------------------------------------------------------
/**
*  This function will attempt to get the SIM ICCID.
*
*  @return EXIT_SUCCESS if the call was successful, EXIT_FAILURE otherwise.
*/
// -------------------------------------------------------------------------------------------------
int cm_sim_GetSimIccid
(
    void
)
{
    char iccid[LE_SIM_ICCID_BYTES];
    le_result_t res;
    le_sim_ObjRef_t simRef = NULL;
    int ret = EXIT_SUCCESS;

    simRef = GetSimRef(SimSlot);

    res = le_sim_GetICCID(simRef, iccid, sizeof(iccid));

    if (res != LE_OK)
    {
        iccid[0] = '\0';
        ret = EXIT_FAILURE;
    }

    cm_cmn_FormatPrint("ICCID", iccid);

    return ret;
}

// -------------------------------------------------------------------------------------------------
/**
*  This function will attempt to get the SIM phone number.
*
*  @return EXIT_SUCCESS if the call was successful, EXIT_FAILURE otherwise.
*/
// -------------------------------------------------------------------------------------------------
int cm_sim_GetSimPhoneNumber
(
    void
)
{
    char number[LE_MDMDEFS_PHONE_NUM_MAX_BYTES];
    le_result_t res;
    le_sim_ObjRef_t simRef = NULL;
    int ret = EXIT_SUCCESS;

    simRef = GetSimRef(SimSlot);

    res = le_sim_GetSubscriberPhoneNumber(simRef, number, sizeof(number));

    if (res != LE_OK)
    {
        number[0] = '\0';
        ret = EXIT_FAILURE;
    }

    cm_cmn_FormatPrint("Phone Number", number);

    return ret;
}

// -------------------------------------------------------------------------------------------------
/**
 *  This function will attempt to get the SIM info (Home PLMN,...).
 *
 *  @return EXIT_SUCCESS if the call was successful, EXIT_FAILURE otherwise.
 */
// -------------------------------------------------------------------------------------------------
int cm_sim_GetSimInfo
(
    void
)
{
    int ret;

    ret = cm_sim_GetNetworkOperator();
    LE_ASSERT(ret == EXIT_SUCCESS);

    ret = cm_sim_GetSimIccid();
    LE_ASSERT(ret == EXIT_SUCCESS);

    cm_sim_GetSimImsi();

    cm_sim_GetSimPhoneNumber();

    return ret;
}

// -------------------------------------------------------------------------------------------------
/**
 *  This function will enter the pin code for the sim.
 *
 *  @return EXIT_SUCCESS if the call was successful, EXIT_FAILURE otherwise.
 */
// -------------------------------------------------------------------------------------------------
int cm_sim_EnterPin
(
    const char * pin    ///< [IN] PIN code
)
{
    le_sim_ObjRef_t simRef = NULL;
    le_result_t     res = LE_OK;

    simRef = GetSimRef(SimSlot);

    res = le_sim_EnterPIN(simRef, pin);

    switch (res)
    {
        case LE_OK:
            printf("Success.\n");
            return EXIT_SUCCESS;
        case LE_NOT_FOUND:
            printf("Failed to select the SIM card for this operation.\n");
            break;
        case LE_OVERFLOW:
            printf("The PIN code is too long (max 8 digits).\n");
            break;
        case LE_UNDERFLOW:
            printf("The PIN code is not long enough (min 4 digits).\n");
            break;
        default:
            printf("Error: %s\n", LE_RESULT_TXT(res));
            printf("Remaining PIN tries: %d\n", le_sim_GetRemainingPINTries(simRef));
            break;
    }

    le_sim_Delete(simRef);

    return EXIT_FAILURE;
}


// -------------------------------------------------------------------------------------------------
/**
 *  This function will change the pin code for the sim.
 *
 *  @return EXIT_SUCCESS if the call was successful, EXIT_FAILURE otherwise.
 */
// -------------------------------------------------------------------------------------------------
int cm_sim_ChangePin
(
    const char * oldPinPtr,     ///< [IN] Old PIN code
    const char * newPinPtr      ///< [IN] New PIN code
)
{
    le_sim_ObjRef_t simRef = NULL;
    le_result_t     res = LE_OK;

    simRef = GetSimRef(SimSlot);

    res = le_sim_ChangePIN(simRef, oldPinPtr, newPinPtr);

    switch(res)
    {
        case LE_OK:
            printf("Success.\n");
            return EXIT_SUCCESS;
        case LE_NOT_FOUND:
            printf("Failed to select the SIM card for this operation.\n");
            break;
        case LE_OVERFLOW:
            printf("The PIN code is too long (max 8 digits).\n");
            break;
        case LE_UNDERFLOW:
            printf("The PIN code is not long enough (min 4 digits).\n");
            break;
        default:
            printf("Error: %s\n", LE_RESULT_TXT(res));
            break;
    }

    return EXIT_FAILURE;
}


// -------------------------------------------------------------------------------------------------
/**
 *  This function will lock the sim.
 *
 *  @return EXIT_SUCCESS if the call was successful, EXIT_FAILURE otherwise.
 */
// -------------------------------------------------------------------------------------------------
int cm_sim_LockSim
(
    const char * pin    ///< [IN] PIN code
)
{
    le_sim_ObjRef_t simRef = NULL;
    le_result_t     res = LE_OK;

    simRef = GetSimRef(SimSlot);

    res = le_sim_Lock(simRef, pin);

    switch (res)
    {
        case LE_OK:
            printf("Success.\n");
            return EXIT_SUCCESS;
        case LE_NOT_FOUND:
            printf("Failed to select the SIM card for this operation.\n");
            break;
        case LE_OVERFLOW:
            printf("The PIN code is too long (max 8 digits).\n");
            break;
        case LE_UNDERFLOW:
            printf("The PIN code is not long enough (min 4 digits).\n");
            break;
        default:
            printf("Error: %s\n", LE_RESULT_TXT(res));
            break;
    }

    le_sim_Delete(simRef);

    return EXIT_FAILURE;
}


// -------------------------------------------------------------------------------------------------
/**
 *  This function will unlock the sim.
 *
 *  @return EXIT_SUCCESS if the call was successful, EXIT_FAILURE otherwise.
 */
// -------------------------------------------------------------------------------------------------
int cm_sim_UnlockSim
(
    const char * pin    ///< [IN] PIN code
)
{
    le_sim_ObjRef_t simRef = NULL;
    le_result_t     res = LE_OK;

    simRef = GetSimRef(SimSlot);

    res = le_sim_Unlock(simRef, pin);

    switch (res)
    {
        case LE_OK:
            printf("Success.\n");
            return EXIT_SUCCESS;
        case LE_NOT_FOUND:
            printf("Failed to select the SIM card for this operation.\n");
            break;
        case LE_OVERFLOW:
            printf("The PIN code is too long (max 8 digits).\n");
            break;
        case LE_UNDERFLOW:
            printf("The PIN code is not long enough (min 4 digits).\n");
            break;
        default:
            printf("Error: %s\n", LE_RESULT_TXT(res));
            printf("Remaining PIN tries: %d\n", le_sim_GetRemainingPINTries(simRef));
            break;
    }

    le_sim_Delete(simRef);

    return EXIT_FAILURE;
}


// -------------------------------------------------------------------------------------------------
/**
 *  This function will unblock the sim.
 *
 *  @return EXIT_SUCCESS if the call was successful, EXIT_FAILURE otherwise.
 */
// -------------------------------------------------------------------------------------------------
int cm_sim_UnblockSim
(
    const char * puk,       ///< [IN] PUK code
    const char * newpin     ///< [IN] New PIN code
)
{
    le_sim_ObjRef_t simRef = NULL;
    le_result_t     res = LE_OK;

    simRef = GetSimRef(SimSlot);

    res = le_sim_Unblock(simRef, puk, newpin);

    switch (res)
    {
        case LE_OK:
            printf("Success.\n");
            return EXIT_SUCCESS;
        case LE_NOT_FOUND:
            printf("Failed to select the SIM card for this operation.\n");
            break;
        case LE_OVERFLOW:
            printf("The PIN code is too long (max 8 digits).\n");
            break;
        case LE_UNDERFLOW:
            printf("The PIN code is not long enough (min 4 digits).\n");
            break;
        case LE_OUT_OF_RANGE:
            printf("The PUK code length is not correct (8 digits).\n");
            break;
        default:
            printf("Error: %s\n", LE_RESULT_TXT(res));
            break;
    }

    le_sim_Delete(simRef);

    return EXIT_FAILURE;
}


// -------------------------------------------------------------------------------------------------
/**
 *  This function will store the pin in the configdb.
 *
 *  @return EXIT_SUCCESS if the call was successful, EXIT_FAILURE otherwise.
 */
// -------------------------------------------------------------------------------------------------
int cm_sim_StorePin
(
    const char * pin    ///< [IN] PIN code
)
{
    char configPath[512];

    snprintf(configPath, sizeof(configPath), "%s/%d", CFG_MODEMSERVICE_SIM_PATH, SimSlot);

    le_cfg_IteratorRef_t iteratorRef = le_cfg_CreateWriteTxn(configPath);
    le_cfg_SetString(iteratorRef, CFG_NODE_PIN, pin);
    le_cfg_CommitTxn(iteratorRef);

    return EXIT_SUCCESS;
}


//--------------------------------------------------------------------------------------------------
/**
* Process commands for sim service.
*/
//--------------------------------------------------------------------------------------------------
void cm_sim_ProcessSimCommand
(
    const char * command,   ///< [IN] Sim commands
    size_t numArgs          ///< [IN] Number of arguments
)
{
    if (strcmp(command, "help") == 0)
    {
        cm_sim_PrintSimHelp();
        exit(EXIT_SUCCESS);
    }
    else if (strcmp(command, "status") == 0)
    {
        exit(cm_sim_GetSimStatus());
    }
    else if (strcmp(command, "enterpin") == 0)
    {
        if (cm_cmn_CheckEnoughParams(1, numArgs, "PIN code missing. e.g. cm sim enterpin <pin>"))
        {
            exit(cm_sim_EnterPin(le_arg_GetArg(2)));
        }
    }
    else if (strcmp(command, "changepin") == 0)
    {
        if (cm_cmn_CheckEnoughParams(2, numArgs, "PIN code missing. e.g. cm sim changepin <pin>"))
        {
            exit(cm_sim_ChangePin(le_arg_GetArg(2), le_arg_GetArg(3)));
        }
    }
    else if (strcmp(command, "lock") == 0)
    {
        if (cm_cmn_CheckEnoughParams(1, numArgs, "PIN code missing. e.g. cm sim lock <pin>"))
        {
            exit(cm_sim_LockSim(le_arg_GetArg(2)));
        }
    }
    else if (strcmp(command, "unlock") == 0)
    {
        if (cm_cmn_CheckEnoughParams(1, numArgs, "PIN code missing. e.g. cm sim unlock <pin>"))
        {
            exit(cm_sim_UnlockSim(le_arg_GetArg(2)));
        }
    }
    else if (strcmp(command, "unblock") == 0)
    {
        if (cm_cmn_CheckEnoughParams(2, numArgs, "PUK/PIN code missing. e.g. cm sim unblock <puk> <newpin>"))
        {
            exit(cm_sim_UnblockSim(le_arg_GetArg(2), le_arg_GetArg(3)));
        }
    }
    else if (strcmp(command, "storepin") == 0)
    {
        if (cm_cmn_CheckEnoughParams(1, numArgs, "PIN code missing. e.g. cm sim storepin <pin>"))
        {
            exit(cm_sim_StorePin(le_arg_GetArg(2)));
        }
    }
    else if (strcmp(command, "info") == 0)
    {
        exit(cm_sim_GetSimInfo());
    }
    else if (strcmp(command, "iccid") == 0)
    {
        exit(cm_sim_GetSimIccid());
    }
    else if (strcmp(command, "imsi") == 0)
    {
        exit(cm_sim_GetSimImsi());
    }
    else if (strcmp(command, "number") == 0)
    {
        exit(cm_sim_GetSimPhoneNumber());
    }
    else
    {
        printf("Invalid command for SIM service.\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
