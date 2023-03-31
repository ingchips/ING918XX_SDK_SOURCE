#ifndef _bluetooth_hci_h
#define _bluetooth_hci_h

//Controller and Baseband Commands
#define HCI_SET_EVT_MASK_CMD_OPCODE                 0x0C01
#define HCI_RESET_CMD_OPCODE                        0x0C03
#define HCI_RD_LE_HOST_SUPP_CMD_OPCODE              0x0C6C
#define HCI_WR_LE_HOST_SUPP_CMD_OPCODE              0x0C6D
#define HCI_RD_AUTH_PAYLOAD_TO_CMD_OPCODE           0x0C7B
#define HCI_WR_AUTH_PAYLOAD_TO_CMD_OPCODE           0x0C7C

//Info Params
#define HCI_RD_LOCAL_VER_INFO_CMD_OPCODE            0x1001
#define HCI_RD_LOCAL_SUPP_CMDS_CMD_OPCODE           0x1002
#define HCI_RD_LOCAL_SUPP_FEATS_CMD_OPCODE          0x1003
#define HCI_RD_LOCAL_EXT_FEATS_CMD_OPCODE           0x1004
#define HCI_RD_BUFF_SIZE_CMD_OPCODE                 0x1005
#define HCI_RD_BD_ADDR_CMD_OPCODE                   0x1009

//Status Params
#define HCI_RD_RSSI_CMD_OPCODE                      0x1405

// LE Commands Opcodes
#define HCI_LE_SET_EVT_MASK_CMD_OPCODE              0x2001
#define HCI_LE_RD_BUFF_SIZE_CMD_OPCODE              0x2002
#define HCI_LE_RD_LOCAL_SUPP_FEATS_CMD_OPCODE       0x2003  // no 0x2004
#define HCI_LE_SET_RAND_ADDR_CMD_OPCODE             0x2005
#define HCI_LE_SET_ADV_PARAM_CMD_OPCODE             0x2006
#define HCI_LE_RD_ADV_CHNL_TX_PW_CMD_OPCODE         0x2007
#define HCI_LE_SET_ADV_DATA_CMD_OPCODE              0x2008
#define HCI_LE_SET_SCAN_RSP_DATA_CMD_OPCODE         0x2009
#define HCI_LE_SET_ADV_EN_CMD_OPCODE                0x200A
#define HCI_LE_SET_SCAN_PARAM_CMD_OPCODE            0x200B
#define HCI_LE_SET_SCAN_EN_CMD_OPCODE               0x200C
#define HCI_LE_CREATE_CON_CMD_OPCODE                0x200D
#define HCI_LE_CREATE_CON_CANCEL_CMD_OPCODE         0x200E
#define HCI_LE_RD_WLST_SIZE_CMD_OPCODE              0x200F
#define HCI_LE_CLEAR_WLST_CMD_OPCODE                0x2010
#define HCI_LE_ADD_DEV_TO_WLST_CMD_OPCODE           0x2011
#define HCI_LE_RMV_DEV_FROM_WLST_CMD_OPCODE         0x2012
#define HCI_LE_CON_UPDATE_CMD_OPCODE                0x2013
#define HCI_LE_SET_HOST_CHNL_CLASSIF_CMD_OPCODE     0x2014
#define HCI_LE_RD_CHNL_MAP_CMD_OPCODE               0x2015
#define HCI_LE_RD_REM_USED_FEATS_CMD_OPCODE         0x2016
#define HCI_LE_ENC_CMD_OPCODE                       0x2017
#define HCI_LE_RAND_CMD_OPCODE                      0x2018
#define HCI_LE_START_ENC_CMD_OPCODE                 0x2019
#define HCI_LE_LTK_REQ_RPLY_CMD_OPCODE              0x201A
#define HCI_LE_LTK_REQ_NEG_RPLY_CMD_OPCODE          0x201B
#define HCI_LE_RD_SUPP_STATES_CMD_OPCODE            0x201C
#define HCI_LE_RX_TEST_CMD_OPCODE                   0x201D
#define HCI_LE_TX_TEST_CMD_OPCODE                   0x201E
#define HCI_LE_TEST_END_CMD_OPCODE                  0x201F
//add in ver4.2
#define HCI_LE_REM_CONN_PARAM_REQ_RPLY_CMD_OPCODE   0x2020
#define HCI_LE_REM_CONN_PARAM_REQ_N_RPLY_CMD_OPCODE 0x2021
#define HCI_LE_SET_DATA_LEN_CMD_OPCODE              0x2022
#define HCI_LE_RD_SUG_DEFAULT_DATA_LEN_CMD_OPCODE   0x2023
#define HCI_LE_WR_SUG_DEFAULT_DATA_LEN_CMD_OPCODE   0x2024
#define HCI_LE_RD_LOCAL_P256_PUBLIC_KEY_CMD_OPCODE  0x2025
#define HCI_LE_GENERATE_DHKEY_CMD_OPCODE            0x2026
#define HCI_LE_ADD_DEV_TO_RESOLV_LIST_CMD_OPCODE    0x2027
#define HCI_LE_RMV_DEV_FROM_RESOLV_LIST_CMD_OPCODE  0x2028
#define HCI_LE_CLEAR_RESOLV_LIST_CMD_OPCODE         0x2029
#define HCI_LE_RD_RESOLV_LIST_SIZE_CMD_OPCODE       0x202A
#define HCI_LE_RD_PEER_RESOLV_ADDR_CMD_OPCODE       0x202B
#define HCI_LE_RD_LOCAL_RESOLV_ADDR_CMD_OPCODE      0x202C
#define HCI_LE_SET_ADDR_RESOLV_EN_CMD_OPCODE        0x202D
#define HCI_LE_SET_RESOLV_PRIV_ADDR_TO_CMD_OPCODE   0x202E
#define HCI_LE_RD_MAX_DATA_LEN_CMD_OPCODE           0x202F

#define HCI_LE_RD_PHY_CMD_OPCODE                        0x2030
#define HCI_LE_SET_DEF_PHY_CMD_OPCODE                   0x2031
#define HCI_LE_SET_PHY_CMD_OPCODE                       0x2032
#define HCI_LE_ENH_RX_TEST_CMD_OPCODE                   0x2033
#define HCI_LE_ENH_TX_TEST_CMD_OPCODE                   0x2034
#define HCI_LE_SET_ADV_SET_ADDR_CMD_OPCODE              0x2035
#define HCI_LE_SET_EXT_ADV_PARAM_CMD_OPCODE             0x2036
#define HCI_LE_SET_EXT_ADV_DATA_CMD_OPCODE              0x2037
#define HCI_LE_SET_EXT_SCAN_RSP_CMD_OPCODE              0x2038
#define HCI_LE_SET_EXT_ADV_EN_CMD_OPCODE                0x2039
#define HCI_LE_RD_MAX_ADV_DATA_LEN_CMD_OPCODE           0x203A
#define HCI_LE_RD_NO_ADV_SETS_CMD_OPCODE                0x203B
#define HCI_LE_RMV_ADV_SET_CMD_OPCODE                   0x203C
#define HCI_LE_CLR_ADV_SETS_CMD_OPCODE                  0x203D
#define HCI_LE_SET_PRD_ADV_PARAM_CMD_OPCODE             0x203E
#define HCI_LE_SET_PRD_ADV_DATA_CMD_OPCODE              0x203F
#define HCI_LE_SET_PRD_ADV_EN_CMD_OPCODE                0x2040
#define HCI_LE_SET_EXT_SCAN_PARAM_CMD_OPCODE            0x2041
#define HCI_LE_SET_EXT_SCAN_EN_CMD_OPCODE               0x2042
#define HCI_LE_EXT_CREATE_CONN_CMD_OPCODE               0x2043
#define HCI_LE_PRD_ADV_CREATE_SYNC_CMD_OPCODE           0x2044
#define HCI_LE_PRD_ADV_CREATE_SYNC_CNCL_CMD_OPCODE      0x2045
#define HCI_LE_PRD_ADV_TERMI_SYNC_CMD_OPCODE            0x2046
#define HCI_LE_ADD_PRD_ADV_LIST_CMD_OPCODE              0x2047
#define HCI_LE_RMV_PRD_ADV_LIST_CMD_OPCODE              0x2048
#define HCI_LE_CLR_PRD_ADV_LIST_CMD_OPCODE              0x2049
#define HCI_LE_RD_PRD_ADV_LIST_SIZE_CMD_OPCODE          0x204A
#define HCI_LE_RD_TX_POWER_CMD_OPCODE                   0x204B
#define HCI_LE_RD_RF_COMP_CMD_OPCODE                    0x204C
#define HCI_LE_WR_RF_COMP_CMD_OPCODE                    0x204D
#define HCI_LE_SET_PRIVACY_MODE_CMD_OPCODE              0x204E

// BLE v5.1 commands
#define HCI_LE_ENH_RX_TEST_V3_CMD_OPCODE                            0x204F
#define HCI_LE_ENH_TX_TEST_V3_CMD_OPCODE                            0x2050
#define HCI_LE_SET_CONNLESS_CTE_TX_PARAM_CMD_OPCODE                 0x2051
#define HCI_LE_SET_CONNLESS_CTE_TX_EN_CMD_OPCODE                    0x2052
#define HCI_LE_SET_CONNLESS_IQ_SAMPLING_EN_CMD_OPCODE               0x2053
#define HCI_LE_SET_CONN_CTE_RX_PARAM_CMD_OPCODE                     0x2054
#define HCI_LE_SET_CONN_CTE_TX_PARAM_CMD_OPCODE                     0x2055
#define HCI_LE_CONN_CTE_REQ_EN_CMD_OPCODE                           0x2056
#define HCI_LE_CONN_CTE_RSP_EN_CMD_OPCODE                           0x2057
#define HCI_LE_READ_ANT_INFO_CMD_OPCODE                             0x2058
#define HCI_LE_SET_PRD_ADV_RX_EN_CMD_OPCODE                         0x2059
#define HCI_LE_PRD_ADV_SYNC_TRANSFER_CMD_OPCODE                     0x205A
#define HCI_LE_PRD_ADV_SET_INFO_TRANSFER_CMD_OPCODE                 0x205B
#define HCI_LE_SET_PRD_ADV_SYNC_TRANSFER_PARAM_CMD_OPCODE           0x205C
#define HCI_LE_SET_DEF_PRD_ADV_SYNC_TRANSFER_PARAM_CMD_OPCODE       0x205D
#define HCI_LE_GEN_DH_KEY_V2_CMD_OPCODE                             0x205E
#define HCI_LE_MODIFY_SLEEP_CLK_ACC_CMD_OPCODE                      0x205F

#define HCI_LE_REQUEST_PEER_SCA_CMD_OPCODE                          0x206D
#define HCI_LE_ENH_TX_TEST_V4_CMD_OPCODE                            0x207B

#endif
