#define RTL8201_REGMAP_H
#ifdef RTL8201_REGMAP_H

/*
#define LAN8742_ANNPTR   ((uint16_t)0x0007U)
#define LAN8742_ANNPRR   ((uint16_t)0x0008U)
#define LAN8742_ENCTR    ((uint16_t)0x0010U)
#define LAN8742_MCSR     ((uint16_t)0x0011U)
#define LAN8742_SMR      ((uint16_t)0x0012U)
#define LAN8742_TPDCR    ((uint16_t)0x0018U)
#define LAN8742_TCSR     ((uint16_t)0x0019U)
#define LAN8742_SECR     ((uint16_t)0x001AU)
#define LAN8742_SCSIR    ((uint16_t)0x001BU)
#define LAN8742_CLR      ((uint16_t)0x001CU)
#define LAN8742_ISFR     ((uint16_t)0x001DU)
#define LAN8742_IMR      ((uint16_t)0x001EU)
#define LAN8742_PHYSCSR  ((uint16_t)0x001FU)
*/

/* Basic Cotnrol & Status Register */
#define RTL8201_BCR     ((uint16_t)0x0000U)
#define RTL8201_BSR     ((uint16_t)0x0001U)

/* PHY Identifier Register */
#define RTL8201_PHYI1R  ((uint16_t)0x0002U)
#define RTL8201_PHYI2R  ((uint16_t)0x0003U)

/* Auto-negotiation Register */
#define RTL8201_ANAR    ((uint16_t)0x0004U)
#define RTL8201_ANLPAR  ((uint16_t)0x0005U)
#define RTL8201_ANER    ((uint16_t)0x0006U)

/* MMD Access Control & Data Register */
#define RTL8201_MMDACR  ((uint16_t)0x000DU)
#define RTL8201_MMDAADR ((uint16_t)0x000EU)

/* RMII Mode Setting Register */
#define RTL8201_RMSR    ((uint16_t)0x0010U)

#endif //RTL8201_REGMAP_H