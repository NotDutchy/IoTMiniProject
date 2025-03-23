import { FaDroplet } from "react-icons/fa6";
import { CiTempHigh } from "react-icons/ci";
import { MdCo2 } from "react-icons/md";

export default function Dashboard() {
    return <div className="flex justify-center items-center bg-gray-100 p-4">
        <div className="card w-80 bg-white shadow-xl p-6 rounded-lg">
            <div className="flex items-center space-x-4">
                <div className="p-3 bg-blue-500 text-white rounded-full">
                    <FaDroplet size={24}/>
                </div>
                <div className="card-body text-black">
                    Here comes the Humidity value!
                </div>
            </div>
        </div>
        <div className="card w-80 bg-white shadow-xl p-6 rounded-lg">
            <div className="flex items-center space-x-4">
                <div className="p-3 bg-blue-500 text-white rounded-full">
                    <CiTempHigh size={24}/>
                </div>
                <div className="card-body text-black">
                    Here comes the temperature value!
                </div>
            </div>
        </div>
        <div className="card w-80 bg-white shadow-xl p-6 rounded-lg">
            <div className="flex items-center space-x-4">
                <div className="p-3 bg-blue-500 text-white rounded-full">
                    <MdCo2 size={24}/>
                </div>
                <div className="card-body text-black">
                    Here comes the CO2 value!
                </div>
            </div>
        </div>
    </div>
}