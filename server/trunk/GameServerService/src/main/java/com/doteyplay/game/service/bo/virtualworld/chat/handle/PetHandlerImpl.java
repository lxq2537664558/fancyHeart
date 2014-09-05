package com.doteyplay.game.service.bo.virtualworld.chat.handle;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.log4j.Logger;

import com.doteyplay.core.dbcs.DBCS;
import com.doteyplay.game.CommonConstants;
import com.doteyplay.game.config.template.SpriteDataObject;
import com.doteyplay.game.constants.IdType;
import com.doteyplay.game.domain.common.IdGenerator;
import com.doteyplay.game.domain.gamebean.PetBean;
import com.doteyplay.game.domain.gamebean.RoleBean;
import com.doteyplay.game.domain.role.Role;
import com.doteyplay.game.persistence.serverdata.pet.IPetBeanDao;
import com.doteyplay.game.persistence.serverdata.role.IRoleBeanDao;
import com.doteyplay.game.service.bo.virtualworld.chat.patameter.PetPatameterObject;
import com.doteyplay.game.util.excel.TemplateService;
/**
 * ���ﴦ����
* @className:PetHandlerImpl.java
* @classDescription:
* @author:Tom.Zheng
* @createTime:2014��8��19�� ����5:46:33
 */
public class PetHandlerImpl extends AbstractHandlerImpl<PetPatameterObject> {

	private static  Logger logger  = Logger.getLogger(PetHandlerImpl.class);
	@Override
	public void onlineHandle(Role role, PetPatameterObject t) {
		// TODO Auto-generated method stub
		for (PetPatameterObject.ItemObject item : t.getItemObjects()) {
			role.getPetManager().addPet(item.itemId);
		}
	}

	@Override
	public void offlineHandle(long roleId, PetPatameterObject t) {
		// TODO Auto-generated method stub
		for (PetPatameterObject.ItemObject item : t.getItemObjects()) {
			IRoleBeanDao roleDao = DBCS.getExector(IRoleBeanDao.class);
			RoleBean selectRoleBean = roleDao.selectRoleBean(roleId);
			if(selectRoleBean==null){
				logger.error("��ɫId������");
				return;
			}
			IPetBeanDao petBeanDao = DBCS.getExector(IPetBeanDao.class);
			List<PetBean> selectedPets = petBeanDao.selectPetBeanListByRoleId(roleId);
			Map<Integer,PetBean> petMap = new HashMap<Integer,PetBean>();
			for (PetBean pet : selectedPets) {
				petMap.put(pet.getSpriteId(), pet);
			}
			if(petMap.containsKey(item.itemId)){
				logger.error("�ó���Idÿ���û�ֻ����һ��,Ŀǰ�Ѿ�����");
				return;
			}
			
			SpriteDataObject petSpriteData = TemplateService.getInstance().get(item.itemId, SpriteDataObject.class);
			if (petSpriteData == null){
				logger.error("�ó���Id �����Ǵ����");
				return;
			}
			PetBean petBean = new PetBean();
			petBean.setAreaId(selectRoleBean.getAreaId());
			petBean.setId(IdGenerator.getInstance().getId(IdType.PET_ID,
					selectRoleBean.getAreaId()));
			petBean.setLevel(1);
			petBean.setStar(petSpriteData.getInitStar());
			petBean.setRoleId(roleId);
			petBean.setSpriteId(item.itemId);
			
			if(petSpriteData.getIsHero() == CommonConstants.TRUE){
				petBean.setGroupStr("0,1,2,3,4");
			}else{
				petBean.setGroupStr("");
			}
			
			petBeanDao.insertPetBean(petBean);
			
			logger.error("GM �� roleId="+roleId+"���ӳ���Id="+item.itemId+"�ɹ�");
			
		}
	}
}