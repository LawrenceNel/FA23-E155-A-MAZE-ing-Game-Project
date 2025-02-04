module decoder (input	logic [15:0] msg,
			   input	logic [2:0] chpnum,
			   output	logic [13:0] chpdata,
			   output	logic [10:0] shftval);
	
	logic [6:0] adrr, adrg;
	logic [13:0] chp1, chp2, chp3, chp4, bm;
	logic [4:0] x;
	logic [1:0] y;
	logic [3:0] rg, en;
	logic id;
	
	assign x = msg[4:0];
	assign y = msg[6:5];
	assign en = msg[10:7];
	assign rg = msg[14:11];
	assign id = msg[15];
	assign bm = {3'b110, 11'b0};
	
	always_comb
		begin
			if (x<=15) begin
				if (y<=1) begin
					chp2 = bm;
					chp3 = bm;
					chp4 = bm;
					
					adrg = 7'h2 * (x) + (y);
					adrr = adrg + 7'h20;
					if (rg) chp1 = {3'b101, adrr, en};
					else chp1 = {3'b101, adrg, en};
				end
				else begin
					chp1 = bm;
					chp2 = bm;
					chp4 = bm;
					
					adrg = 7'h2 * (x) + (y - 2'd2);
					adrr = adrg + 7'h20;
					if (rg) chp3 = {3'b101, adrr, en};
					else chp3 = {3'b101, adrg, en};
				end
			end
			else begin
				if (y<=1) begin
					chp1 = bm;
					chp3 = bm;
					chp4 = bm;
					
					adrg = 7'h2 * (x - 5'd16) + (y);
					adrr = adrg + 7'h20;
					if (rg) chp2 = {3'b101, adrr, en};
					else chp2 = {3'b101, adrg, en};
				end
				else begin
					chp1 = bm;
					chp2 = bm;
					chp3 = bm;
					
					adrg = 7'h2 * (x - 5'd16) + (y - 2'd2);
					adrr = adrg + 7'h20;
					if (rg) chp4 = {3'b101, adrr, en};
					else chp4 = {3'b101, adrg, en};
				end
			end
		end
	
	always_comb
		if (~id) begin
			case (chpnum)
				3'b001:		chpdata = chp1;
				3'b010: 	chpdata = chp2;
				3'b011: 	chpdata = chp3;
				3'b100: 	chpdata = chp4;
				default:	chpdata = chp1;
			endcase
			shftval = 13;
		end
		else begin
			shftval = 11;
			chpdata = {3'b100, msg[14:6], 2'b00};
		end
	
endmodule