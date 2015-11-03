//Transport

SendData(destination, data);
RecieveData(destination, data);

//Network

SendSegment(destination, segment){
	case status:
		RecepientNotAvailable
			return 0;
		RecepientAvailableDelivered
			return 1;
		RecepientAvailableNotDelivered
			return 2;
		Timeout
			return 3;
}
RecieveSegment(destination, segment);

//Link

