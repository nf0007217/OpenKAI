#include "HM_base.h"

namespace kai
{

HM_base::HM_base()
{
	m_pCAN = NULL;
	m_pCMD = NULL;
//	m_pGPS = NULL;
	m_strCMD = "";
	m_rpmL = 0;
	m_rpmR = 0;
	m_motorRpmW = 0;
	m_bSpeaker = false;
	m_bMute = false;
	m_canAddrStation = 0x301;

	m_maxRpmT = 65535;
	m_maxRpmW = 2500;
	m_ctrlB0 = 0;
	m_ctrlB1 = 0;
	m_defaultRpmT = 3000;
	m_wheelR = 0.1;
	m_treadW = 0.4;

	m_pinLEDl = 11;
	m_pinLEDm = 12;
	m_pinLEDr = 13;

	m_dT.init();
	m_dRot.init();
	m_dir = dir_forward;
}

HM_base::~HM_base()
{
}

bool HM_base::init(void* pKiss)
{
	IF_F(!this->ActionBase::init(pKiss));
	Kiss* pK = (Kiss*)pKiss;
	pK->m_pInst = this;

	F_INFO(pK->v("maxSpeedT", &m_maxRpmT));
	F_INFO(pK->v("maxSpeedW", &m_maxRpmW));
	F_INFO(pK->v("motorRpmW", &m_motorRpmW));
	F_INFO(pK->v("defaultRpmT", &m_defaultRpmT));
	F_INFO(pK->v("wheelR", &m_wheelR));
	F_INFO(pK->v("treadW", &m_treadW));
	F_INFO(pK->v("bMute", &m_bMute));
	F_INFO(pK->v("canAddrStation", &m_canAddrStation));
	F_INFO(pK->v("pinLEDl", (int*)&m_pinLEDl));
	F_INFO(pK->v("pinLEDm", (int*)&m_pinLEDm));
	F_INFO(pK->v("pinLEDr", (int*)&m_pinLEDr));

	Kiss* pI = pK->o("cmd");
	IF_T(pI->empty());
	m_pCMD = new _TCP();
	F_ERROR_F(m_pCMD->init(pI));

	return true;
}

bool HM_base::link(void)
{
	IF_F(!this->ActionBase::link());
	Kiss* pK = (Kiss*)m_pKiss;

	string iName;

	iName = "";
	F_ERROR_F(pK->v("_Canbus", &iName));
	m_pCAN = (_Canbus*) (pK->root()->getChildInstByName(&iName));

//	iName = "";
//	F_ERROR_F(pK->v("_GPS", &iName));
//	m_pGPS = (_GPS*) (pK->root()->getChildInstByName(&iName));

	return true;
}

void HM_base::update(void)
{
	this->ActionBase::update();
	NULL_(m_pAM);
	NULL_(m_pCMD);

	if(m_rpmL > m_rpmR)
		m_dir = dir_right;
	else if(m_rpmL < m_rpmR)
		m_dir = dir_left;
	else
		m_dir = dir_forward;

	updateGPS();
	updateCAN();

	string* pStateName = m_pAM->getCurrentStateName();

	if(*pStateName == "HM_STANDBY" || *pStateName == "HM_STATION" || *pStateName=="HM_FOLLOWME")
	{
		m_rpmL = 0;
		m_rpmR = 0;
	}
	else
	{
		m_rpmL = m_defaultRpmT;
		m_rpmR = m_defaultRpmT;
	}

	m_motorRpmW = 0;
	m_bSpeaker = false;

	cmd();
}

void HM_base::updateGPS(void)
{
//	NULL_(m_pGPS);

	const static double tBase = 1.0/(USEC_1SEC*60.0);

	//force rpm to only rot or translation at a time
	if(abs(m_rpmL) != abs(m_rpmR))
	{
		int mid = (m_rpmL + m_rpmR)/2;
		int absRpm = abs(m_rpmL - mid);

//		if(m_rpmL > m_rpmR)
//		{
//			m_rpmL = absRpm;
//			m_rpmR = -absRpm;
//		}
//		else
//		{
//			m_rpmL = -absRpm;
//			m_rpmR = absRpm;
//		}

		m_dT.z = 0.0;
		m_dRot.x = 360.0 * (((double)m_rpmL) * tBase * m_wheelR * 2 * PI) / (m_treadW * PI);
	}
	else if(m_rpmL != m_rpmR)
	{
		m_dT.z = 0.0;
		m_dRot.x = 360.0 * (((double)m_rpmL) * tBase * m_wheelR * 2 * PI) / (m_treadW * PI);
	}
	else
	{
		m_dT.z = ((double)m_rpmL) * tBase * m_wheelR * 2 * PI;
		m_dRot.x = 0.0;
	}

	//TODO:Temporal
	m_dT.z *= 1000000;

//	m_pGPS->setSpeed(&m_dT,&m_dRot);
//	LOG_I("dZ="<<m_dT.z<<" dYaw="<<m_dRot.x);
}

void HM_base::cmd(void)
{
	if (!m_pCMD->isOpen())
	{
		m_pCMD->open();
		return;
	}

	char buf;
	while (m_pCMD->read((uint8_t*) &buf, 1) > 0)
	{
		if (buf == ',')break;
		IF_CONT(buf==LF);
		IF_CONT(buf==CR);

		m_strCMD += buf;
	}

	IF_(buf!=',');

	LOG_I("CMD: "<<m_strCMD);

	string stateName;
	if(m_strCMD=="start")
	{
		stateName = *m_pAM->getCurrentStateName();

		if(stateName=="HM_STATION")
		{
			stateName = "HM_KICKBACK";
			m_pAM->transit(&stateName);
		}
		else if(stateName=="HM_STANDBY")
		{
			m_pAM->transit(m_pAM->getLastStateIdx());
		}
	}
	else if(m_strCMD=="stop")
	{
		stateName = "HM_STANDBY";
		m_pAM->transit(&stateName);
	}
	else if(m_strCMD=="work")
	{
		stateName = "HM_WORK";
		m_pAM->transit(&stateName);
	}
	else if(m_strCMD=="back_to_home")
	{
		stateName = "HM_RTH";
		m_pAM->transit(&stateName);
	}
	else if(m_strCMD=="follow_me")
	{
		stateName = "HM_FOLLOWME";
		m_pAM->transit(&stateName);
	}
	else if(m_strCMD=="station")
	{
		stateName = "HM_STATION";
//		m_pGPS->reset();
		m_pAM->transit(&stateName);
	}

	m_strCMD = "";
}

void HM_base::updateCAN(void)
{
	NULL_(m_pCAN);

	//send
	if(m_bMute)m_bSpeaker = false;

	unsigned long addr = 0x113;
	unsigned char cmd[8];

	m_ctrlB0 = 0;
	m_ctrlB0 |= (m_rpmR<0)?(1 << 4):0;
	m_ctrlB0 |= (m_rpmL<0)?(1 << 5):0;
	m_ctrlB0 |= (m_motorRpmW<0)?(1 << 6):0;

	uint16_t motorPwmL = abs(constrain(m_rpmL, m_maxRpmT, -m_maxRpmT));
	uint16_t motorPwmR = abs(constrain(m_rpmR, m_maxRpmT, -m_maxRpmT));
	uint16_t motorPwmW = abs(constrain(m_motorRpmW, m_maxRpmW, -m_maxRpmW));

	m_ctrlB1 = 0;
	m_ctrlB1 |= 1;						//tracktion motor relay
	m_ctrlB1 |= (1 << 1);				//working motor relay
	m_ctrlB1 |= (m_bSpeaker)?(1 << 5):0;//speaker
	m_ctrlB1 |= (1 << 6);				//external command
	m_ctrlB1 |= (1 << 7);				//0:pwm, 1:rpm

	uint16_t bFilter = 0x00ff;

	cmd[0] = m_ctrlB0;
	cmd[1] = m_ctrlB1;
	cmd[2] = motorPwmL & bFilter;
	cmd[3] = (motorPwmL>>8) & bFilter;
	cmd[4] = motorPwmR & bFilter;
	cmd[5] = (motorPwmR>>8) & bFilter;
	cmd[6] = motorPwmW & bFilter;
	cmd[7] = (motorPwmW>>8) & bFilter;

	m_pCAN->send(addr, 8, cmd);

	//status LED
	string stateName = *m_pAM->getCurrentStateName();
	if(stateName=="HM_WORK" || stateName=="HM_RTH")
	{
		m_pCAN->pinOut(m_pinLEDl,0);
		m_pCAN->pinOut(m_pinLEDm,1);
		m_pCAN->pinOut(m_pinLEDr,0);
	}
	else if(stateName=="HM_FOLLOWME")
	{
		m_pCAN->pinOut(m_pinLEDl,1);
		m_pCAN->pinOut(m_pinLEDm,0);
		m_pCAN->pinOut(m_pinLEDr,0);
	}
	else if(stateName=="HM_STANDBY")
	{
		m_pCAN->pinOut(m_pinLEDl,0);
		m_pCAN->pinOut(m_pinLEDm,0);
		m_pCAN->pinOut(m_pinLEDr,1);
	}
	else if(stateName=="HM_KICKBACK")
	{
		m_pCAN->pinOut(m_pinLEDl,1);
		m_pCAN->pinOut(m_pinLEDm,1);
		m_pCAN->pinOut(m_pinLEDr,1);
	}
	else
	{
		m_pCAN->pinOut(m_pinLEDl,0);
		m_pCAN->pinOut(m_pinLEDm,0);
		m_pCAN->pinOut(m_pinLEDr,0);
	}

	//receive
	uint8_t* pCanData = m_pCAN->get(m_canAddrStation);
	NULL_(pCanData);

	//CAN-ID301h 1byte 4bit "AC-input"
	//1:ON-Docking/0:OFF-area
	IF_(!((pCanData[1] >> 4) & 1));
	IF_(stateName == "HM_KICKBACK");

	stateName = "HM_STATION";
//	m_pGPS->reset();
	m_pAM->transit(&stateName);

}

bool HM_base::draw(void)
{
	IF_F(!this->ActionBase::draw());
	Window* pWin = (Window*) this->m_pWindow;
	Mat* pMat = pWin->getFrame()->getCMat();
	NULL_F(pMat);
	IF_F(pMat->empty());

	string msg = *this->getName() + ": rpmL=" + i2str(m_rpmL)
			+ ", rpmR=" + i2str(m_rpmR);
	pWin->addMsg(&msg);

	return true;
}



}
